#include "transportiface.h"
#include <QJsonDocument>


/************ Supply *************/
/*** auth data ***/

AuthData::AuthData(QString user, QString pass) :
    user(user), pass(pass)
{}

QString AuthData::getUser() const
{
    return user;
}

QString AuthData::getPass() const
{
    return pass;
}

void AuthData::setUser(const QString &value)
{
    user = value;
}

void AuthData::setPass(const QString &value)
{
    pass = value;
}

QJsonObject AuthData::toJson() const
{
    QJsonObject json;
    json["user"] = user;
    json["pass"] = pass;
    return json;
}


/*** netpoint ***/

NetPoint::NetPoint(const QHostAddress &addr, quint16 port) :
    addr(addr), port(port)
{}

const QHostAddress &NetPoint::getAddr() const
{
    return addr;
}

quint16 NetPoint::getPort() const
{
    return port;
}

void NetPoint::setPort(quint16 value)
{
    port = value;
}


/****** client basics ******/

NetTransport::NetTransport()
{
    this->moveToThread(&thread);
    thread.start();
}

NetTransport::~NetTransport()
{
    thread.quit();
    thread.wait();
}


/************ simple client *************/

SimpleTcpClient::SimpleTcpClient() :
    NetTransport(auth, parent), netpoint(netpoint)
{

    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(soketDisconneted()));
}

SimpleTcpClient::~SimpleTcpClient()
{
    delete socket;
}

QTcpSocket *SimpleTcpClient::getTcpSocket() const
{
    return socket;
}

bool SimpleTcpClient::connectToServer()
{
    connRequired = true;
    socket->connectToHost(netpoint.getAddr(), netpoint.getPort());
    while(socket->state() == QAbstractSocket::SocketState::ConnectingState);
    return  (socket->state() == QAbstractSocket::SocketState::ConnectedState);
}

void SimpleTcpClient::disconnectFromServer()
{
    connRequired = false;
    socket->disconnectFromHost();
}

int SimpleTcpClient::send(const QJsonObject &object)
{
    QJsonObject authObject, docObj;
    this->auth.write(authObject);

    docObj = object;
    docObj["auth"] = authObject;

    QJsonDocument doc(docObj);
    return socket->write(doc.toJson());
}

int SimpleTcpClient::read(QJsonObject &)
{
    //todo
    return 0;
}

int SimpleTcpClient::isMessageAvailable()
{
    //todo
    return 0;
}

void SimpleTcpClient::soketDisconneted()
{
    emit disconnectedFromHost();
    while(connRequired && (socket->state() != QAbstractSocket::SocketState::ConnectedState)){
        socket->connectToHost(netpoint.getAddr(), netpoint.getPort());
        while(socket->state() == QAbstractSocket::SocketState::ConnectingState);
    }
}

