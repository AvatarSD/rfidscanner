#include "networkclient.h"


NetworkClient::NetworkClient(NetTransport *transport, QObject *parent) :
    Eventful (parent), transport(transport)
{

}

void NetworkClient::run()
{

}

void NetworkClient::setAuthData(const AuthData &auth)
{
    this->auth = auth;
}
