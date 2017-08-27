#include "transportiface.h"

TransportIface::TransportIface(QObject *parent) : QObject(parent)
{

}

SimpleTcpJsonClient::SimpleTcpJsonClient(QObject *parent) : TransportIface(parent)
{

}
