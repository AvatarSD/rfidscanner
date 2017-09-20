#include "networkclient.h"


NetworkClient::NetworkClient(NetClient *transport, QObject *parent) :
    QObject (parent), transport(transport)
{

}

void NetworkClient::run()
{

}
