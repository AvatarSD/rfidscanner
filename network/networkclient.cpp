#include "networkclient.h"


NetworkClient::NetworkClient(Transport *transport, QObject *parent) :
    QObject (parent), transport(transport)
{

}

void NetworkClient::run()
{

}
