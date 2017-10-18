#ifndef B_PROTOCOL_H
#define B_PROTOCOL_H

#include "a_phy.h"


/********************** Level 2(B) ***********************/

/*************** Interface ****************/

/******* NetProtocol *******/
class NetProtocol : public Eventful
{
    Q_OBJECT
public:
    enum NetProtocolParseErr{
        PARSE_ERR_OK = 0,        // -> return QByteArrat with payload
        PARSE_ERR_CHECKSUM = 1,  //
        PARSE_ERR_LENGH = 2,     // -> delete buff after TAIL
        PARSE_ERR_NO_PAYLOAD = 3,
        PARSE_ERR_NO_END = 4,  // -> delete buff till start
        PARSE_ERR_NOTHING = 5    // -> delete buff till (buffSize - (headerLength - 1)) elem
    };
    NetProtocol(QObject*parent=nullptr) : Eventful(parent){}
    virtual ~NetProtocol(){}

    template<class ForwardIt1, class ForwardIt2>
    static ForwardIt1 search(ForwardIt1 & first, ForwardIt1 last,
                             ForwardIt2 s_first, ForwardIt2 s_last);
public slots:
    virtual QByteArray pack (QByteArray msg) = 0;
    virtual QByteArray parse(QByteArray raw,
                             NetProtocolParseErr *err = nullptr) = 0;
};


/************ Implementation **************/

/**** NetProtocolBound *****/
class NetProtocolBound
{
public:
    typedef quint32 PayloadLengh;
    typedef quint16 PayloadCrc;

    NetProtocolBound(const QByteArray &header,
                      const QByteArray &tail)
    {setHeader(header); setTail(tail);}

    QByteArray header() const;
    QByteArray length(PayloadLengh payloadLength) const;
    QByteArray crc(PayloadCrc payloadCrc) const;
    QByteArray tail() const;

    uint headerSize() const;
    uint lengthSize() const;
    uint crcSize() const;
    uint tailSize() const;

    void setHeader(const QByteArray &header);
    void setTail(const QByteArray &tail);

private:
    QByteArray _header;
    QByteArray _tail;
    uint _headerSize;
    uint _tailSize;
};

/***** ByteArrayQueue ******/
class ByteArrayQueue
{
public:
    typedef QQueue<QByteArray> ByteQueue;
    typedef ByteQueue::iterator FirstLevelIt;
    typedef ByteQueue::value_type::iterator SecondLevelIt;

    class iterator : public std::iterator<std::input_iterator_tag, char>
    {
    public:
        // construct iterator at begin
        iterator(ByteQueue &data, SecondLevelIt firstPos) :
            data(data), fit(data.begin()), sit(firstPos){}
        // construct iterator at end
        iterator(ByteQueue& data) :
            data(data), fit(data.end()), sit(data.end()->end())
        {}
        inline char operator*() const {return *sit; }
        inline bool operator==(const iterator& other) const {
            return ((fit == other.fit) && (fit == data.end() || sit == other.sit));
        }
        inline bool operator!=(const iterator& other) const {
            return !this->operator ==(other);
        }
        inline iterator& operator ++() {
            if (fit == data.end())
                return *this;
            ++sit;
            if (sit >= fit->end()){
                ++fit;
                if (fit != data.end())
                    sit = fit->begin();
            }
            return *this;
        }
    private:
        ByteQueue& data;
        FirstLevelIt fit;
        SecondLevelIt sit;
    };
    inline iterator begin(){ return iterator(data, firstPos); }
    inline iterator end(){ return iterator(data); }
    void enqueue(const QByteArray &t);
    void removeUntill(const iterator& el);
private:
    ByteQueue data;
    SecondLevelIt firstPos; // no resize the Qbytearray, just hold first byte addr;
};

/* NetProtocolBoundV1Queue */
class NetProtocolBoundV1Queue : public NetProtocol
{
    /* Message bound format:
     *    ${HEADER} ${LENGTH} ${PAYLOAD} ${CRC16} ${ENDLINE}
     * Where:
     *  - ${LENGTH} - uint32 - size of ${PAYLOAD}
     *  - ${CRC16} - uint16 - crc-16 of ${PAYLOAD}
     */
    Q_OBJECT
public:
    NetProtocolBoundV1Queue(const NetProtocolBound &format,
                       QObject*parent=nullptr) :
        NetProtocol(parent), format(format) {}
    virtual ~NetProtocolBoundV1Queue(){}

public slots:
    virtual QByteArray pack(QByteArray msg);
    virtual QByteArray parse(QByteArray raw,
                             NetProtocolParseErr *err = nullptr);
private:
    const NetProtocolBound format;
    ByteArrayQueue inBuf;
};

/**** NetProtocolBoundV1 ***/
class NetProtocolBoundV1 : public NetProtocol
{
    Q_OBJECT
public:
    enum ParserState{
        START, LENGTH, DATA
    };
    NetProtocolBoundV1(const NetProtocolBound &format,
                       QObject*parent=nullptr) :
        NetProtocol(parent), format(format), state(START) {}
    virtual ~NetProtocolBoundV1(){}

public slots:
    virtual QByteArray pack(QByteArray msg);
    virtual QByteArray parse(QByteArray raw, NetProtocolParseErr *err
                             = nullptr);
private:
    const NetProtocolBound format;
    QByteArray buff;
    ParserState state;
};


#endif // B_PROTOCOL_H
