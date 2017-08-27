#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H


class NetworkClient
{
public:
    NetworkClient();
    virtual ~NetworkClient(){}

    virtual void run();
};


class PollingClient : public NetworkClient
{
    void run();
};

class EventClient : public NetworkClient
{
    void run();
};

class RpcClient : public NetworkClient
{
    void run();
};

#endif // NETWORKCLIENT_H
