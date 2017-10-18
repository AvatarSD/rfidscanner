#include "b_protocol.h"
#include <QDataStream>
#include <functional>


/********************** Level 2(B) ***********************/

/*************** Interface ****************/

/******* NetProtocol *******/
template<class ForwardIt1, class ForwardIt2>
ForwardIt1 NetProtocol::search(ForwardIt1 &first, ForwardIt1 last,
                               ForwardIt2 s_first, ForwardIt2 s_last)
{
    for (; ; ++first) {
        ForwardIt1 it = first;
        for (ForwardIt2 s_it = s_first; ; ++it, ++s_it) {
            if (s_it == s_last) /* found - all element are equal*/
                return first;
            if (it == last)     /* not found - et end()*/
                return last;
            if (*it != *s_it)   /* first is not equeal - go to next*/
                break;
        }
    }
}


/************ Implementation **************/

/**** NetProtocolFormat ****/
uint NetProtocolBound::headerSize() const{
    return _headerSize;
}
uint NetProtocolBound::lengthSize() const{
    return sizeof(PayloadLengh);
}
uint NetProtocolBound::crcSize() const{
    return sizeof(PayloadCrc);
}
uint NetProtocolBound::tailSize() const{
    return _tailSize;
}
QByteArray NetProtocolBound::header() const{
    return _header;
}
QByteArray NetProtocolBound::tail() const{
    return _tail;
}
QByteArray NetProtocolBound::length(NetProtocolBound::PayloadLengh payloadLength) const
{
    QByteArray out;
    out.reserve(lengthSize()+2); // +2 just for safe
    QDataStream stream(&out, QIODevice::WriteOnly);
    stream << payloadLength;
    return out;
}
QByteArray NetProtocolBound::crc(NetProtocolBound::PayloadCrc payloadCrc) const
{
    QByteArray out;
    out.reserve(crcSize()+2); // +2 just for safe
    QDataStream stream(&out, QIODevice::WriteOnly);
    stream << payloadCrc;
    return out;
}
void NetProtocolBound::setHeader(const QByteArray &header)
{
    _header = header;
    _headerSize = _header.size();
}
void NetProtocolBound::setTail(const QByteArray &tail)
{
    _tail = tail;
    _tailSize = _tail.size();
}

/***** ByteArrayQueue ******/
void ByteArrayQueue::enqueue(const QByteArray &t){
    data.enqueue(t);
    if(data.size()==1)
        firstPos = data.begin()->begin();
}
void ByteArrayQueue::removeUntill(const ByteArrayQueue::iterator &el)
{
    if(el == end()){
        data.clear();
        return;
    }
    forever{
        if(begin() == el)
            return;
        ++firstPos;
        if(firstPos == data.begin()->end()){
            data.dequeue();
            firstPos = data.begin()->begin();
        }
    };
}

/* NetProtocolV1FormatQueue */
QByteArray NetProtocolV1FormatQueue::pack(QByteArray msg)
{
    QByteArray out;
    out.reserve(format.headerSize() +
                format.lengthSize() +
                msg.size() +
                format.crcSize() +
                format.tailSize());
    out += format.header();
    out += format.length(msg.size());
    out += msg;
    out += format.crc(qChecksum(msg.data(),msg.size()));
    out += format.tail();
    return out;
}
QByteArray NetProtocolV1FormatQueue::parse(QByteArray raw, NetProtocolParseErr *err)
{
    auto errToStr = [](NetProtocolParseErr error) -> QString {
        switch (error) {
        case PARSE_ERR_OK: return QStringLiteral("Parse OK.");
        case PARSE_ERR_CHECKSUM: return QStringLiteral("Invalid message checksum.");
        case PARSE_ERR_LENGH: return QStringLiteral("Invalid message length.");
        case PARSE_ERR_NO_PAYLOAD: return QStringLiteral("Invalid message payload size.");
        case PARSE_ERR_NO_END: return QStringLiteral("Tail not found. Maybe it's just a chunck of data?");
        case PARSE_ERR_NOTHING: return QStringLiteral("Nothing to parse.");
        }
        return QStringLiteral("Other error.");
    };

    auto ret = [&](NetProtocolParseErr error,
            ByteArrayQueue::iterator * tailItP = nullptr,
            QByteArray dat = QByteArray()) -> QByteArray {
        if(error < PARSE_ERR_NO_END)
            if(tailItP != nullptr)
                inBuf.removeUntill(*tailItP);
        if(err!=nullptr)
            *err = error;
        if(error >= PARSE_ERR_NO_END)
            emit sysEvent(QSharedPointer<Event> (
                              new NetworkEvent(
                                  NetworkEvent::INFO,
                                  NetworkEvent::PARSER_ERROR,
                                  QStringLiteral("Code ") + QString::number(error) + ": " +
                                  errToStr(error))));
        else if (error > PARSE_ERR_OK)
            emit sysEvent(QSharedPointer<Event> (
                              new NetworkEvent(
                                  NetworkEvent::WARNING,
                                  NetworkEvent::PARSER_ERROR,
                                  QStringLiteral("Code ") + QString::number(error) + ": " +
                                  errToStr(error))));
        else if (error == PARSE_ERR_OK)
            emit sysEvent(QSharedPointer<Event> (
                              new NetworkEvent(
                                  NetworkEvent::DEBUG,
                                  NetworkEvent::PARSER_ERROR,
                                  QStringLiteral("Code ") + QString::number(error) + ": " +
                                  errToStr(error)+ " All is ok!")));
        return dat;
    };

    /* append data to buf */
    inBuf.enqueue(raw);

    /* try find the header */
    auto msgIt  = inBuf.begin();
    auto headerIt = search(msgIt, inBuf.end(),
                           format.header().begin(), format.header().end());

    /* remove unvalid data from buff */
    inBuf.removeUntill(msgIt);

    /* test */
    if(headerIt == inBuf.end())
        return ret(PARSE_ERR_NOTHING);

    /* try find the tail */
    auto tailIt = search(msgIt, inBuf.end(),
                         format.tail().begin(), format.tail().end());
    if(tailIt == inBuf.end())
        return ret(PARSE_ERR_NO_END);

    /* check min msg size */
    int size = std::distance(headerIt, tailIt)-
            format.headerSize()-
            format.lengthSize()-
            format.crcSize();
    if(size < 1) return ret(PARSE_ERR_NO_PAYLOAD, &tailIt);

    /* iterate to length*/
    for(uint i = 0;i<format.headerSize();i++, ++headerIt);

    /* read length */
    QByteArray len;
    len.reserve(format.lengthSize());
    for(uint i = 0;i<format.lengthSize();i++, ++headerIt)
        len.append(*headerIt);
    uint32_t lenVal = *((uint32_t*)len.data());
    if((int)lenVal != size) return ret(PARSE_ERR_LENGH, &tailIt);

    /* copying data */
    QByteArray data;
    data.reserve(size+format.crcSize());
    std::copy(headerIt,tailIt,data.begin());
    QByteArray crc = data.right(format.crcSize());
    uint16_t crcVal = *((uint16_t*)crc.data());
    data.chop(format.crcSize());
    if(qChecksum(data.data(),data.size()) != crcVal)
        return ret(PARSE_ERR_CHECKSUM, &tailIt);

    /* ret ok */
    return ret(PARSE_ERR_OK, &tailIt, data);

    return QByteArray();
}

/**** NetProtocolV1Format ***/
QByteArray NetProtocolV1Format::pack(QByteArray msg)
{
    QByteArray out;
    out.reserve(format.headerSize() +
                format.lengthSize() +
                msg.size() +
                format.crcSize() +
                format.tailSize());
    out += format.header();
    out += format.length(msg.size());
    out += msg;
    out += format.crc(qChecksum(msg.data(),msg.size()));
    out += format.tail();
    return out;
}
QByteArray NetProtocolV1Format::parse(QByteArray raw, NetProtocolParseErr *err)
{
    auto errToStr = [](NetProtocolParseErr error) -> QString {
        switch (error) {
        case PARSE_ERR_OK: return QStringLiteral("Parse OK.");
        case PARSE_ERR_CHECKSUM: return QStringLiteral("Invalid message checksum.");
        case PARSE_ERR_LENGH: return QStringLiteral("Invalid message length.");
        case PARSE_ERR_NO_PAYLOAD: return QStringLiteral("Invalid message payload size.");
        case PARSE_ERR_NO_END: return QStringLiteral("Tail not found. Maybe it's just a chunck of data?");
        case PARSE_ERR_NOTHING: return QStringLiteral("Nothing to parse.");
        }
        return QStringLiteral("Other error.");
    };

    auto ret = [&](NetProtocolParseErr error, QByteArray dat = QByteArray()) -> QByteArray
    {
        if(err!=nullptr)
            *err = error;
        if(error >= PARSE_ERR_NO_END)
            emit sysEvent(QSharedPointer<Event> (
                              new NetworkEvent(
                                  NetworkEvent::INFO,
                                  NetworkEvent::PARSER_ERROR,
                                  QStringLiteral("Code ") + QString::number(error) + ": " +
                                  errToStr(error))));
        else if (error > PARSE_ERR_OK)
            emit sysEvent(QSharedPointer<Event> (
                              new NetworkEvent(
                                  NetworkEvent::WARNING,
                                  NetworkEvent::PARSER_ERROR,
                                  QStringLiteral("Code ") + QString::number(error) + ": " +
                                  errToStr(error))));
        else if (error == PARSE_ERR_OK)
            emit sysEvent(QSharedPointer<Event> (
                              new NetworkEvent(
                                  NetworkEvent::DEBUG,
                                  NetworkEvent::PARSER_ERROR,
                                  QStringLiteral("Code ") + QString::number(error) + ": " +
                                  errToStr(error)+ " All is ok!")));
        return dat;
    };

    int start = 0;
    switch (state) {
    case START:{
        start = raw.indexOf(format.header());
        if(start < 0)
            return ret(PARSE_ERR_NOTHING);
        start += format.headerSize();
        buff.clear();
        state = LENGTH;
    }
    case LENGTH:{
        uint readed = buff.size();
        buff += raw.mid(start,format.lengthSize()-readed);
        if(buff.size() < (int)format.lengthSize())
            return ret(PARSE_ERR_NO_END);
        start += format.lengthSize()-readed;
        QDataStream stream(&buff, QIODevice::ReadOnly);
        quint32 length;
        stream >> length;
        buff.clear();
        buff.reserve(length+format.crcSize()+format.tailSize());
        state = DATA;
    }
    case DATA:{
        buff += raw.mid(start);
        int tail = buff.indexOf(format.tail());
        if(tail < 0)
            return ret(PARSE_ERR_NO_END);
        state = START;
        if(tail <= (int)format.crcSize())
            return ret(PARSE_ERR_NO_PAYLOAD);
        auto crcArr = buff.mid(tail-format.crcSize(),format.crcSize());
        buff.truncate(tail-format.crcSize());
        QDataStream streamCrc(&crcArr, QIODevice::ReadOnly);
        quint16 crc;
        streamCrc >> crc;
        if(crc != qChecksum(buff.data(),buff.size()))
            return ret(PARSE_ERR_CHECKSUM);
        return ret(PARSE_ERR_OK, buff);
    }
    }
    return QByteArray();
}


