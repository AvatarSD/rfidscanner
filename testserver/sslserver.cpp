#include "sslserver.h"

void SslServer::incomingConnection(qintptr socketDescriptor)
{
    QSslSocket *serverSocket = new QSslSocket();
    if (serverSocket->setSocketDescriptor(socketDescriptor)){
      addPendingConnection (serverSocket);
    }
    else{
      delete serverSocket;
    }
}
