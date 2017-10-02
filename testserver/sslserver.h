#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QSslError>
#include <QTcpServer>
#include <QSslSocket>

// QTcpServer extension to intercept new socket connection and enable SSL
class SslServer : public QTcpServer
{
  Q_OBJECT

public:
  virtual void incomingConnection(qintptr socketDescriptor);
};

#endif // SSLSERVER_H
