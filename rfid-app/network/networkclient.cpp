#include "networkclient.h"


NetworkClient::NetworkClient(NetClient *transport, QObject *parent) :
    Eventianle (parent), transport(transport)
{

}

void NetworkClient::run()
{

}
