#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include "transportiface.h"
#include <QObject>
#include <types.h>


class AuthData : public Serialaizeable
{
public:
    AuthData(QString user, QString pass);
    QString getUser() const;
    QString getPass() const;
    void setUser(const QString &value);
    void setPass(const QString &value);
    virtual QJsonObject toJson() const;
private:
    QString user, pass;
};


class NetworkClient : public Eventful
{
    Q_OBJECT
public:
    NetworkClient(NetTransport * transport, QObject *parent = nullptr);
    virtual ~NetworkClient(){}
public slots:
    virtual void run();
    virtual void netEventOut(QSharedPointer<Event>);

        void setAuthData(const AuthData & auth);
    // void makeAnswer();
    // MsgID whatAnswerID();

protected:
    NetTransport * transport;
    AuthData auth;
};


/* need to set:
 *  *NetTransport
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
