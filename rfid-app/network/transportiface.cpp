#include "transportiface.h"
#include <QJsonDocument>


/************ Supply *************/


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



/******************** Implementation *********************/

/***** NetTransport ******/

/* simple client */
SimpleTcpClient::SimpleTcpClient()
{

    QObject::connect(&socket, SIGNAL(disconnected()), this, SLOT(soketDisconneted()));
}

SimpleTcpClient::~SimpleTcpClient()
{

}

const QTcpSocket *SimpleTcpClient::getTcpSocket() const
{
    return &socket;
}

bool SimpleTcpClient::connectToHost(QString host)
{
    connRequired = true;
//    socket.connectToHost(netpoint.getAddr(), netpoint.getPort());
//    while(socket.state() == QAbstractSocket::SocketState::ConnectingState);
//    return  (socket.state() == QAbstractSocket::SocketState::ConnectedState);
}

void SimpleTcpClient::disconnectFromHost()
{
    connRequired = false;
    socket.disconnectFromHost();
}

NetTransport::NetTransportState SimpleTcpClient::state() const
{

}
uint32_t SimpleTcpClient::send(QByteArray data)
{
//    QJsonObject authObject, docObj;
//    this->auth.write(authObject);

//    docObj = object;
//    docObj["auth"] = authObject;

//    QJsonDocument doc(docObj);
//    return socket.write(doc.toJson());
}

//int SimpleTcpClient::read(QJsonObject &)
//{
//    //todo
//    return 0;
//}



void SimpleTcpClient::soketDisconneted()
{
//    emit disconnectedFromHost();
//    while(connRequired && (socket.state() != QAbstractSocket::SocketState::ConnectedState)){
//        socket.connectToHost(netpoint.getAddr(), netpoint.getPort());
//        while(socket.state() == QAbstractSocket::SocketState::ConnectingState);
//    }
}

