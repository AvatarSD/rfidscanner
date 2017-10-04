#include "transportiface.h"
#include <QTcpSocket>


/************ Supply *************/

/******* NetPoint ********/
NetPoint::NetPoint() : _addr(QStringLiteral("")), _port(0){
}
NetPoint::NetPoint(QString addr, quint16 port) : _addr(addr), _port(port){
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


/****** client basics ******/
NetTransport::NetTransport(){
    this->moveToThread(&thread);
    thread.start();
}
NetTransport::~NetTransport(){
    thread.quit();
    thread.wait();
}



/******************** Implementation *********************/

/***** NetTransport ******/

/* simple client */
SimpleTcpClient::SimpleTcpClient() : SimpleTcpClient(new QTcpSocket(this)){
}
SimpleTcpClient::SimpleTcpClient(QAbstractSocket *socket) :
    socket(socket), zerotimer(this)
{

    connect(socket, SIGNAL(readyRead()),
            this, SLOT(socketReadyRead()));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(&zerotimer, SIGNAL(timeout()),
            this, SLOT(run()));
    zerotimer.start(0);
}
SimpleTcpClient::~SimpleTcpClient(){
    zerotimer.stop();
}
const QAbstractSocket *SimpleTcpClient::getSocket() const{
    return socket.data();
}

bool SimpleTcpClient::connectToHost(const NetPoint &host)
{
    if(!host.isNull())
        this->host = host;
//    socket.connectToHost(netpoint.getAddr(), netpoint.getPort());
//    while(socket.state() == QAbstractSocket::SocketState::ConnectingState);
//    return  (socket.state() == QAbstractSocket::SocketState::ConnectedState);
}

void SimpleTcpClient::disconnectFromHost()
{
    socket->disconnectFromHost();
}

qint32 SimpleTcpClient::send(QByteArray data)
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    return socket->write(data);
    else return 0;
}

void SimpleTcpClient::socketReadyRead()
{
    emit recv(socket->readAll());
}

void SimpleTcpClient::socketStateChanged(QAbstractSocket::SocketState state)
{

}

void SimpleTcpClient::socketError(QAbstractSocket::SocketError error)
{

}

NetTransport::NetTransportState SimpleTcpClient::currentState() const
{

}

void SimpleTcpClient::run()
{

}



//void SimpleTcpClient::soketDisconneted()
//{
//    emit disconnectedFromHost();
//    while(connRequired && (socket.state() != QAbstractSocket::SocketState::ConnectedState)){
//        socket.connectToHost(netpoint.getAddr(), netpoint.getPort());
//        while(socket.state() == QAbstractSocket::SocketState::ConnectingState);
//    }
//}

