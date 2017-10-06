#ifndef TRANSPORTIFACE_H
#define TRANSPORTIFACE_H

#include <QString>
#include <QAbstractSocket>
#include <QScopedPointer>
#include <QTimer>
#include <QThread>
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

/******* NetPoint ********/
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

/***** NetState ******/
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

/***** NetTransport ******/
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

/***** NetProtocol ******/
class NetProtocol : public Eventful
{
    Q_OBJECT
public:
    enum NetProtocolParseErr{
        /* Byte     |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
         * order:   |              |crop|more|  error code  |
         */
        PARSE_ERR_OK                  = 0b00000,
        PARSE_ERR_NOTHING             = 0b00001,
        PARSE_ERR_NO_START            = 0b00010, // -> crop data after "\r\n\r\n"
        PARSE_ERR_LENGH               = 0b00011, // -> crop data after "\r\n\r\n"
        PARSE_ERR_CHECKSUM            = 0b00100, // -> crop data after "\r\n\r\n"S
        PARSE_MORE                    = 0b01000, //for bitmask, intend to repeat parse with new data
        PARSE_CROPPED                 = 0b10000, //for bitmask, delete unusable data
    };
    NetProtocol(QObject*parent=nullptr) : Eventful(parent){}
    virtual ~NetProtocol(){}
    virtual QByteArray parse(QByteArray raw, NetProtocolParseErr & err) = 0;
    virtual QByteArray pack (QByteArray msg) = 0;
};



/******************** Implementation *********************/

/***** NetTransport ******/
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


/***** NetProtocol ******/












#endif // TRANSPORTIFACE_H
