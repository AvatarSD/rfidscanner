#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include "transportiface.h"
#include <QObject>
#include <types.h>

class NetworkClient : public QObject
{
    Q_OBJECT
public:
    NetworkClient(NetClient * transport, QObject *parent = nullptr);
    virtual ~NetworkClient(){}
public slots:
    virtual void run();
//    virtual void

protected:
    NetClient * transport;
};


/* need to set:
 *  *NetClient
 *  **ip and port endpoint
 *  **
 */

class EventClient : public NetworkClient
{
    virtual void run();
};

class PollingClient : public NetworkClient
{
    virtual void run();
};

class RpcClient : public NetworkClient
{
    virtual void run();
};

#endif // NETWORKCLIENT_H
