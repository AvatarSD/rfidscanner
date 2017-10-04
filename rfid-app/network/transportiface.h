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
    _________________________________________________________________________________________________________
    2. BoundProt:       - Pack RAW message to              - Incoming buffer(circular
                          RAW with boundary;                 or not, queue);
                        - Bound-detection in               - Start/end boundary
                          incoming RAW bytes                 bytes pattern;
                          queue for parse. Give            - CRC check;
                          RAW msg with clean bound;        - Eventful iface;
    _________________________________________________________________________________________________________
    3. NetMsg:          - Incapsulate network              - QJSonObject as main
                          message as object;                 data;
                        - Packing itself to RAW;           - Pack() method;
                        - Static factory method            - Get/Set to json field;
                          which parse RAW data and         - Child must implement
                          create new NetMsg obj;             exec(ScannerFacade*) for
                        - Ready to inherits for              impl server request.
                          implement any action on
                          ScannerFacade interface;
    _________________________________________________________________________________________________________
    4. Commander:       - Queue with outgoing NetMsg       - Queue with associated       - Event/Pool mode;
                          which require a reply.             NetMsg, timestamp and       - Auth data;
                        - Measure server response            some flags;
                          time, event creation time        - Eventful iface;
                          to now duration(for debug);      - Interface for receiving
                        - Searching UUID from                Event objects;
                          incomming msg in outgoing        - Statistic data;
                          queue. If a match is found,      - Pointers to: BoundProt
                          the corresponding outgoing         (with NetTransport),
                          msg will have been delete.         ScannerFacade;
                        - In EVENT mode all message
                          in outgoing queue will send
                          automatically. In POOL mode
                          it will happenen by correct
                          request(aka NetMsg);
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


/***** Pack/Unpack general message; Autoconnection ******/

class NetTransport : public Eventful
{
    Q_OBJECT
public:
    NetTransport();
    virtual ~NetTransport();
signals:
    void read(QByteArray msg);
    void connectedToHost();
    void disconnectedFromHost();
public slots:
    virtual void send(QByteArray msg) = 0; //by mutex
    virtual bool connectToHost() = 0;
    virtual void disconnectFromHost() = 0;
    virtual bool isConnected() = 0;
protected:
    QThread thread;

};


/**************** Impl *****************/

class NetPoint
{
public:
    NetPoint(const QHostAddress &addr, quint16 port);

    const QHostAddress &getAddr() const;
    void setPort(quint16 value);
    quint16 getPort() const;

private:
    QHostAddress addr;
    quint16 port;
};


class SimpleTcpClient : public NetTransport
{
    Q_OBJECT
public:
    SimpleTcpClient();
    ~SimpleTcpClient();

public slots:
    virtual void send(QSharedPointer<NetworkMessage> msg);
    virtual bool connectToHost();
    virtual void disconnectFromHost();
//    QTcpSocket *getTcpSocket() const;

private slots:
    void soketDisconneted();
    //    void error(QAbstractSocket::SocketError socketError);
    //    void stateChanged(QAbstractSocket::SocketState socketState);

private:
    QTcpSocket socket;
    const NetPoint netpoint;
    bool connRequired;
};



#endif // TRANSPORTIFACE_H
