#ifndef TRANSPORTIFACE_H
#define TRANSPORTIFACE_H

#include <QString>
#include <QAbstractSocket>
#include <QScopedPointer>
#include <QTimer>
#include <QThread>
#include <QQueue>
#include <iterator>
#include "messages.h"


/************************************************************************************************************
 --- Stack ---

    *prog* *prog* *prog*
         \    |    /
        - commander -
              |
         - message -
              |
       - general msg -
              |
        - transport -
              |
         *hardware*

*************************************************************************************************************
 --- Detail ---

    Layer:              Should for:                        Contain:                      Settings:
    _________________________________________________________________________________________________________
    1. NetTransport:    - Can be: SslTransport,            - TcpSocket obj, eth;         - Connection point;
                          Binary, rs485, can, etc;         - Server net point;
                        - Auto re-connection;              - Own thread for I/O;
                        - State changed signal;            - Eventful iface;
                        - DNS lookup
    _________________________________________________________________________________________________________
    2. NetProtocol:     - Pack RAW message to              - Incoming buffer(circular
                          RAW with boundary;                 or not, queue);
                        - Bound-detection in               - Start/end boundary
                          incoming RAW bytes                 bytes pattern;
                          queue for parse. Give            - CRC check;
                          RAW msg with clean bound;        - Eventful iface;
    _________________________________________________________________________________________________________
    3. NetMessage:      - Incapsulate network              - QJSonObject as main
                          message as object;                 data;
                        - Packing itself to RAW;           - Pack() method;
                        - Static factory method            - Get/Set to json field;
                          which parse RAW data and         - Child must implement
                          create new NetMsg obj;             exec(ScannerFacade*) for
                        - Ready to inherits for              impl server request.
                          implement any action on
                          ScannerFacade interface;
    _________________________________________________________________________________________________________
    4. NetCommander:    - Queue with outgoing NetMsg       - Queue with associated       - Event/Pool mode;
                          which require a reply.             NetMsg, timestamp and       - Auth data;
                        - Measure server response            some flags;
                          time, event creation time        - Eventful iface;
                          to now duration(for debug);      - Interface for receiving
                        - Searching UUID from                Event objects;
                          incomming msg in outgoing        - Statistic data;
                          queue. If a match is found,      - Pointers to: BoundProt
                          the corresponding outgoing         (with NetTransport),
                          msg will have been delete.         ScannerFacade;
                        - In EVENT mode all message        - Own thread for msg
                          in outgoing queue will send        parsing/pakcing,
                          automatically. In POOL mode        executing (block
                          it will happenen by correct        operation execution
                          request(aka NetMsg);               in separete thread)
                        - Authentication module;
                        - Call execute() in separate
                          new thread if require. In
                          conclusion new outgoing
                          NetMsg will be created or
                          something else will be
                          happen;

*************************************************************************************************************
 --- Note ---
   1. Besides settings above, all network mobule must have the
      choice of implementation for: NetTransport, BoundProt;

************************************************************************************************************/


/********************** Interface ************************/

/**************** Level 1 *****************/

/******** NetPoint *********/
class NetPoint{
public:
    NetPoint();
    NetPoint(QString addr, quint16 port);
    QString addr() const;
    quint16 port() const;
    void addr(QString addr);
    void port(quint16 port);
    bool isNull() const;
    void reset();
private:
    QString _addr;
    quint16 _port;
};
Q_DECLARE_METATYPE(NetPoint)

/******** NetState *********/
class NetState
{
public:
    NetState(){}
    NetState(QAbstractSocket::SocketState state);
    QString toString();
    NetState & operator = (QAbstractSocket::SocketState state);
    QAbstractSocket::SocketState _state;
};
Q_DECLARE_METATYPE(NetState)

/****** NetTransport *******/
class NetTransport : public Eventful
{
    Q_OBJECT
public:
    NetTransport(QObject* parent = nullptr) :
        Eventful(parent) {qRegisterMetaType<NetState>();}
    virtual ~NetTransport(){}
signals:
    void recv(QByteArray data);
    void stateChanged(NetState newState);
public slots:
    /* only quened connections! */
    virtual qint32 send(QByteArray data) = 0;
    virtual void connectToHost(NetPoint addr = NetPoint()) = 0;
    virtual void disconnectFromHost() = 0;
    virtual NetState currentState() const = 0;
};

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



/******************** Implementation *********************/

/**************** Level 1 *****************/

/****** NetTransport *******/
class TcpNetTransport : public NetTransport
{
    Q_OBJECT
public:
    TcpNetTransport(QObject* parent = nullptr);
    TcpNetTransport(QAbstractSocket * socket, QObject* parent = nullptr);
    ~TcpNetTransport();
public slots:
    const QAbstractSocket* getSocket() const;
    virtual NetState currentState() const;
    virtual void connectToHost(NetPoint addr = NetPoint());
    virtual void disconnectFromHost();
    virtual qint32 send(QByteArray data);
protected slots:
    void socketReadyRead();
    void run();
    void socketStateChanged(QAbstractSocket::SocketState state);
    void socketError(QAbstractSocket::SocketError error);
protected:
    QScopedPointer<QAbstractSocket> socket;
    QTimer zerotimer;
    NetPoint host;
    bool reconnectRequired;
};


/**************** Level 2 *****************/

/**** NetProtocolFormat ****/
class NetProtocolFormat
{
public:
    typedef quint32 PayloadLengh;
    typedef quint16 PayloadCrc;

    NetProtocolFormat(const QByteArray &header,
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

/**** NetProtocolV1Bound ***/
class NetProtocolV1Bound : public NetProtocol
{
    /* Message bound format:
     *    ${HEADER} ${LENGTH} ${PAYLOAD} ${CRC16} ${ENDLINE}
     * Where:
     *  - ${LENGTH} - uint32 - size of ${PAYLOAD}
     *  - ${CRC16} - uint16 - crc-16 of ${PAYLOAD}
     */
    Q_OBJECT
public:
    NetProtocolV1Bound(const NetProtocolFormat &format,
                       QObject*parent=nullptr) :
        NetProtocol(parent), format(format) {}
    virtual ~NetProtocolV1Bound(){}

public slots:
    virtual QByteArray pack(QByteArray msg);
    virtual QByteArray parse(QByteArray raw,
                             NetProtocolParseErr *err = nullptr);
private:
    const NetProtocolFormat format;
    ByteArrayQueue inBuf;
};

/**** NetProtocolV2Bound ***/
class NetProtocolV2Bound : public NetProtocol
{
    Q_OBJECT
public:
    enum ParserState{
        START, LENGTH, DATA
    };
    NetProtocolV2Bound(const NetProtocolFormat &format,
                       QObject*parent=nullptr) :
        NetProtocol(parent), format(format), state(START) {}
    virtual ~NetProtocolV2Bound(){}

public slots:
    virtual QByteArray pack(QByteArray msg);
    virtual QByteArray parse(QByteArray raw, NetProtocolParseErr *err
                             = nullptr);
private:
    const NetProtocolFormat format;
    QByteArray buff;
    ParserState state;
};






#endif // TRANSPORTIFACE_H
