#ifndef TRANSPORTIFACE_H
#define TRANSPORTIFACE_H

#include <QIODevice>

class TransportIface : public QObject
{
    Q_OBJECT
public:
    explicit TransportIface(QIODevice & device, QObject *parent = nullptr);
    virtual ~TransportIface(){}

public slots:
    virtual bool connect();
    virtual bool disconnect();

    virtual int send(const QJsonDocument & doc);
    virtual int read(QJsonDocument & doc);

    virtual int isMessageAvailable();

signals:
    void disconnected();

//private:
//    QDataStream stream;

};

/*********************/
// transportIfase  coding and bundle data in to the stream,
// determine protocol of top level.
/*********************/
// stack:
//  -pack data stream-
//         |
//  -work with device-
/*********************/
// todo: write and draw stack at paper!!!
/*********************/
// SslTransport, Binary, rs485, can, etc
/*********************/

class SimpleTcpClient : public TransportIface
{
    Q_OBJECT
public:
    SimpleTcpJsonClient(QObject *parent = nullptr);

protected slots:
//    void error(QAbstractSocket::SocketError socketError);
//    void stateChanged(QAbstractSocket::SocketState socketState);
//    void disconnected();

private:
    QTcpSocket *tcpSocket;

};



#endif // TRANSPORTIFACE_H
