#include <QCoreApplication>
#include <QSslSocket>
#include <server.h>

#define USE_PORT 4443

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server;
    server.loadCertAndKey("cert.pem", "key.pem");
    server.start(USE_PORT);

    return a.exec();
}
