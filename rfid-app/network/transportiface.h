#ifndef TRANSPORTIFACE_H
#define TRANSPORTIFACE_H

#include <QString>
#include <QJsonObject>
#include <QTcpSocket>
#include <QHostAddress>
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
                        - Connected/Disconnected           - Eventful iface;
                          signals;
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

/***** NetTransport ******/
class NetTransport : public Eventful
{
    Q_OBJECT
public:
    enum NetTransportState{
        /* require a signal */
        NET_CONNECTED,
        NET_DISCONNECTED,
        NET_ABOUT_TO_CLOSE,
        /* just by call state() */
        NET_HOST_LOOCKUP,
        NET_RECONNECTING,
        NET_CONNECTING,
    };
    NetTransport();
    virtual ~NetTransport();
signals:
    void recv(QByteArray data);
    void stateChanged(NetTransportState newState);
public slots:
    /* only quened connections! */
    virtual uint32_t send(QByteArray data) = 0;
    virtual bool connectToHost(QString host) = 0;
    virtual void disconnectFromHost() = 0;
    virtual NetTransportState state() const = 0;
protected:
    QThread thread;
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
    virtual ~NetProtocol(){}
    virtual QByteArray parse(QByteArray raw, NetProtocolParseErr & err) = 0;
    virtual QByteArray pack (QByteArray msg) = 0;
};


/******************** Implementation *********************/

/***** NetTransport ******/
class SimpleTcpClient : public NetTransport
{
    Q_OBJECT
public:
    SimpleTcpClient();
    ~SimpleTcpClient();
    const QTcpSocket *getTcpSocket() const;
public slots:
    virtual uint32_t send(QByteArray data);
    virtual bool connectToHost(QString host);
    virtual void disconnectFromHost();
    virtual NetTransportState state() const;
    // all signals:
    //   void recv(QByteArray data);
    //   void sysEv(QSharedPointer<Event>);
    //   void stateChanged(NetState newState);
protected slots:
    void soketDisconneted();
protected:
    QTcpSocket socket;
    QString host;
    bool connRequired;
    NetTransportState linkState;
};

/***** NetProtocol ******/












#endif // TRANSPORTIFACE_H
