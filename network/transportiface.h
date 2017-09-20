#ifndef TRANSPORTIFACE_H
#define TRANSPORTIFACE_H

#include <QIODevice>
#include <QString>
#include <QJsonObject>
#include <QTcpSocket>
#include <QHostAddress>


class AuthData
{
public:
    AuthData(QString user, QString pass);

    QString getUser() const;
    QString getPass() const;

    void write(QJsonObject &json) const;

private:
    const QString user;
    const QString pass;
};


class NetClient : public QObject
{
    Q_OBJECT
public:
    explicit NetClient(const AuthData & auth, QObject *parent = nullptr);
    virtual ~NetClient(){}

public slots:
    virtual bool connect(){return false;}
    virtual bool disconnect(){return false;}

    virtual int send(const QJsonDocument &){return 0;}
    virtual int read(QJsonDocument &){return 0;}

    virtual int isMessageAvailable(){return 0;}

signals:
    void connectedToHost();
    void disconnectedFromHost();

protected:
    const AuthData auth;
    //    QDataStream stream;

};

/********************
 transportIfase  coding and bundle data in to the stream,
 determine protocol of top level.
*********************
 stack:
  -pack data stream-
         |
  -work with device-
*********************
todo: write and draw stack at paper!!!
*********************
SslTransport, Binary, rs485, can, etc
*********************/

class NetPoint
{
public:
    NetPoint(const QHostAddress &addr, quint16 port);

    const QHostAddress &getAddr() const;
    quint16 getPort() const;
    void setPort(quint16 value);

private:
    QHostAddress addr;
    quint16 port;
};

class SimpleTcpClient : public NetClient
{
    Q_OBJECT
public:
    SimpleTcpClient(const NetPoint & netpoint, const AuthData &auth, QObject *parent = nullptr);
    ~SimpleTcpClient();

    QTcpSocket *getTcpSocket() const;

public slots:
    bool connectToServer();
    void disconnectFromServer();
    int send(const QJsonObject &object);
    int read(QJsonObject &);
    int isMessageAvailable();

private slots:
    void soketDisconneted();
    //    void error(QAbstractSocket::SocketError socketError);
    //    void stateChanged(QAbstractSocket::SocketState socketState);

private:
    const NetPoint netpoint;
    bool connRequired;
    QTcpSocket *socket;


};



#endif // TRANSPORTIFACE_H
