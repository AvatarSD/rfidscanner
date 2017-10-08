#include "transportiface.h"
#include <QTcpSocket>
#include <QDataStream>
#include <functional>

#include "iostream"


/********************** Interface ************************/

/**************** Level 1 *****************/

/********* Supply **********/
#define RECONNECT_TIME 50
#define RECONNECT_ERR_TIME 1500
#define RECONNECT_CRI_TIME 10000
#define RECONNECT_EXT_TIME 100000

/******** NetPoint *********/
NetPoint::NetPoint() : _addr(), _port(0){
    qRegisterMetaType<NetPoint>();
}
NetPoint::NetPoint(QString addr, quint16 port) : _addr(addr), _port(port){
    qRegisterMetaType<NetPoint>();
}
QString NetPoint::addr() const{
    return _addr;
}
void NetPoint::addr(QString addr){
    _addr = addr;
}
quint16 NetPoint::port() const{
    return _port;
}
void NetPoint::port(quint16 port){
    _port = port;
}
bool NetPoint::isNull() const{
    return (_addr.isNull() && (_port == 0));
}
void NetPoint::reset(){
    _addr.clear(); _port = 0;
}

/******** NetState *********/
NetState::NetState(QAbstractSocket::SocketState state) : _state(state){
}
QString NetState::toString(){
    switch (_state) {
    case QAbstractSocket::UnconnectedState: return QStringLiteral("Unconnected");
    case QAbstractSocket::HostLookupState: return QStringLiteral("HostLookup");
    case QAbstractSocket::ConnectingState: return QStringLiteral("Connecting");
    case QAbstractSocket::ConnectedState: return QStringLiteral("Connected");
    case QAbstractSocket::BoundState: return QStringLiteral("Bound");
    case QAbstractSocket::ListeningState: return QStringLiteral("Listening");
    case QAbstractSocket::ClosingState: return QStringLiteral("Closing");
    default: return QStringLiteral("Undefined"); }
}
NetState &NetState::operator =(QAbstractSocket::SocketState state){
    this->_state = state;
    return *this;
}

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



/******************** Implementation *********************/

/**************** Level 1 *****************/

/****** NetTransport *******/
TcpNetTransport::TcpNetTransport(QObject* parent) :
    TcpNetTransport(new QTcpSocket(),parent){
    socket->setParent(this);
}
TcpNetTransport::TcpNetTransport(QAbstractSocket *socket, QObject *parent) :
    NetTransport(parent), socket(socket), zerotimer(this)
{
    qRegisterMetaType<QAbstractSocket::SocketState>();
    qRegisterMetaType<QAbstractSocket::SocketError>();
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(socketReadyRead()));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(&zerotimer, SIGNAL(timeout()),
            this, SLOT(run()));
    zerotimer.setTimerType(Qt::CoarseTimer);
    zerotimer.setSingleShot(true);
    zerotimer.start(RECONNECT_TIME);
}
TcpNetTransport::~TcpNetTransport(){
    zerotimer.stop();
}
const QAbstractSocket *TcpNetTransport::getSocket() const{
    return socket.data();
}
NetState TcpNetTransport::currentState() const{
    return socket->state();
}
/**** IO operations ****/
qint32 TcpNetTransport::send(QByteArray data){
    if(socket->state() == QAbstractSocket::ConnectedState)
        return socket->write(data);
    else return -1;
}
void TcpNetTransport::socketReadyRead(){
    emit recv(socket->readAll());
}
/**** connection estab. operations ****/
void TcpNetTransport::connectToHost(NetPoint addr){
    reconnectRequired = true;
    if(!addr.isNull())
        this->host = addr;
    socket->connectToHost(this->host.addr(), this->host.port());
}
void TcpNetTransport::disconnectFromHost(){
    reconnectRequired = false;
    socket->disconnectFromHost();
}
/* keepalive functionality */
void TcpNetTransport::run()
{
    if((socket->state() == QAbstractSocket::UnconnectedState) && reconnectRequired)
        connectToHost();
    zerotimer.start(RECONNECT_TIME);
}
void TcpNetTransport::socketStateChanged(QAbstractSocket::SocketState state)
{
    QString msg;
    switch(state){
    case QAbstractSocket::UnconnectedState:
        msg = "Socket unconnected. Last address was: " +
                host.addr() + ":" + QString::number(host.port());
        break;
    case QAbstractSocket::HostLookupState:
        msg = "Attempt to connect to: " + host.addr() + ":" +
                QString::number(host.port());
        break;
    case QAbstractSocket::ConnectingState:
        msg = host.addr() + " look-up'ed. Try to connect to " +
                QString::number(host.port()) + " port.";
        break;
    case QAbstractSocket::ConnectedState:
        msg = "Connected to: " + host.addr() + ":" +
                QString::number(host.port());
        break;
    case QAbstractSocket::ClosingState:
        msg = "Socket is about to close";
        break;
    case QAbstractSocket::BoundState:
    case QAbstractSocket::ListeningState:
    default:
        msg = "Socket go to unexpected state. =(("; break;
    }
    emit sysEvent(QSharedPointer<Event> (
                      new NetworkEvent(NetworkEvent::INFO,
                                       NetworkEvent::IDs::SOCKET_STATE, msg)));
    emit stateChanged(state);
}
void TcpNetTransport::socketError(QAbstractSocket::SocketError error)
{
    uint intv = RECONNECT_TIME;
    NetworkEvent::EventLevel lvl = NetworkEvent::ERROR;
    switch (error)
    {
    case QAbstractSocket::DatagramTooLargeError:
    case QAbstractSocket::UnfinishedSocketOperationError:
    case QAbstractSocket::OperationError:
    case QAbstractSocket::TemporaryError:
        /* nor a error*/
        lvl = NetworkEvent::WARNING;
        break;
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::SocketTimeoutError:
    case QAbstractSocket::NetworkError:
    case QAbstractSocket::ProxyConnectionClosedError: // noe
    case QAbstractSocket::ProxyConnectionTimeoutError:  // noe
        /* not critical error -> do reconnect */
        break;
    case QAbstractSocket::ConnectionRefusedError:  // 1000ms   Ex
    case QAbstractSocket::SocketResourceError:     // noe 1000ms
    case QAbstractSocket::AddressInUseError:  // noe
    case QAbstractSocket::SocketAddressNotAvailableError: // noe
    case QAbstractSocket::SslHandshakeFailedError:  // 1000ms   Ex
    case QAbstractSocket::UnknownSocketError:  // noe
    case QAbstractSocket::UnsupportedSocketOperationError:
    case QAbstractSocket::ProxyConnectionRefusedError: // noe
        /* critical error -> reconnnect after wait time*/
        intv = RECONNECT_ERR_TIME;
        break;
    case QAbstractSocket::HostNotFoundError:       // 10000ms
    case QAbstractSocket::SslInvalidUserDataError:
    case QAbstractSocket::SslInternalError:     // 100000
    case QAbstractSocket::SocketAccessError:
    case QAbstractSocket::ProxyNotFoundError:    // 10000ms
    case QAbstractSocket::ProxyProtocolError:   // 10000
        /* critical error -> do not reconnect */
        intv = RECONNECT_CRI_TIME;
        lvl = NetworkEvent::CRITICAL;
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        /*for future*/
        intv = RECONNECT_EXT_TIME;
        break;
    }
    emit sysEvent(QSharedPointer<Event> (
                      new NetworkEvent(
                          lvl,NetworkEvent::SOCKET_ERROR,
                          QStringLiteral("Code ") + QString::number(error) + ": " +
                          socket->errorString() +
                          (intv==RECONNECT_TIME ? "." : ". Sleep for " +
                                                  QString::number(intv) + "ms."))));
    zerotimer.start(intv);
}



/**************** Level 2 *****************/

/**** NetProtocolFormat ****/
uint NetProtocolFormat::headerSize() const{
    return _headerSize;
}
uint NetProtocolFormat::lengthSize() const{
    return sizeof(PayloadLengh);
}
uint NetProtocolFormat::crcSize() const{
    return sizeof(PayloadCrc);
}
uint NetProtocolFormat::tailSize() const{
    return _tailSize;
}
QByteArray NetProtocolFormat::header() const{
    return _header;
}
QByteArray NetProtocolFormat::tail() const{
    return _tail;
}
QByteArray NetProtocolFormat::length(NetProtocolFormat::PayloadLengh payloadLength) const
{
    QByteArray out;
    out.reserve(lengthSize()+2); // +2 just for safe
    QDataStream stream(&out, QIODevice::WriteOnly);
    stream << payloadLength;
    return out;
}
QByteArray NetProtocolFormat::crc(NetProtocolFormat::PayloadCrc payloadCrc) const
{
    QByteArray out;
    out.reserve(crcSize()+2); // +2 just for safe
    QDataStream stream(&out, QIODevice::WriteOnly);
    stream << payloadCrc;
    return out;
}
void NetProtocolFormat::setHeader(const QByteArray &header)
{
    _header = header;
    _headerSize = _header.size();
}
void NetProtocolFormat::setTail(const QByteArray &tail)
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
void ByteArrayQueue::removeUntill(const ByteArrayQueue::iterator &el) //TODO
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

/**** NetProtocolV1Bound ***/
QByteArray NetProtocolV1Bound::pack(QByteArray msg)
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
QByteArray NetProtocolV1Bound::parse(QByteArray raw, NetProtocolParseErr *err)
{
    try{
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
    }
    catch(std::exception exp){
        std::cout << exp.what() << std::endl;
    }
    return QByteArray();
}

/**** NetProtocolV2Bound ***/
QByteArray NetProtocolV2Bound::pack(QByteArray msg)
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

QByteArray NetProtocolV2Bound::parse(QByteArray raw, NetProtocol::NetProtocolParseErr *err)
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
        buff += raw.right(start);
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

























