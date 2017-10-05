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
    qRegisterMetaType<NetTransport::NetTransportState>();
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
SimpleTcpClient::SimpleTcpClient() : SimpleTcpClient(new QTcpSocket){
}
SimpleTcpClient::SimpleTcpClient(QAbstractSocket *socket) : socket(socket)
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
    zerotimer.start(0);
}
SimpleTcpClient::~SimpleTcpClient(){
    zerotimer.stop();
}
const QAbstractSocket *SimpleTcpClient::getSocket() const{
    return socket.data();
}
NetTransport::NetTransportState SimpleTcpClient::currentState() const{
    return state;
}



/**** IO operations ****/
qint32 SimpleTcpClient::send(QByteArray data){
    if(socket->state() == QAbstractSocket::ConnectedState)
        return socket->write(data);
    else return -1;
}
void SimpleTcpClient::socketReadyRead(){
    emit recv(socket->readAll());
}



/**** connection estab. operations ****/
bool SimpleTcpClient::connectToHost(const NetPoint &host){
    if(!host.isNull())
        this->host = host;
    socket->connectToHost(host.addr(), host.port());
    return true;
}
void SimpleTcpClient::disconnectFromHost(){
    socket->disconnectFromHost();
}


void SimpleTcpClient::socketStateChanged(QAbstractSocket::SocketState state)
{

    switch (state) {
    case QAbstractSocket::UnconnectedState :
        break;
    case QAbstractSocket::HostLookupState :
        break;
    case QAbstractSocket::ConnectingState :
        break;
    case QAbstractSocket::ConnectedState :
        break;
    case QAbstractSocket::ClosingState :
        break;


        break;
    default:
        break;
    }

}

void SimpleTcpClient::run()
{



}

void SimpleTcpClient::socketError(QAbstractSocket::SocketError error)
{
    emit sysEv(QSharedPointer<InfoEvent> (
                   new NetworkEvent(
                       NetworkEvent::WARNING,
                       (NetworkEvent::IDs)error,
                       socket->errorString())));;


}

void SimpleTcpClient::setState(NetTransport::NetTransportState state)
{
    emit sysEv(QSharedPointer<InfoEvent> (
                   new NetworkEvent(
                       NetworkEvent::INFO,
                       (NetworkEvent::IDs)state,
                       QStringLiteral("state changed"))));;


}



//void SimpleTcpClient::soketDisconneted()
//{
//    emit disconnectedFromHost();
//    while(connRequired && (socket.state() != QAbstractSocket::SocketState::ConnectedState)){
//        socket.connectToHost(netpoint.getAddr(), netpoint.getPort());
//        while(socket.state() == QAbstractSocket::SocketState::ConnectingState);
//    }
//}

