#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include "transportiface.h"
#include <QObject>
#include <types.h>


/********************** Interface ************************/

/******* Auth Data *******/
class AuthData : public Serialaizeable
{
public:
    AuthData();
    AuthData(QString user, QString pass);
    QString getUser() const;
    QString getPass() const;
    void setUser(const QString &value);
    void setPass(const QString &value);
    virtual QJsonObject toJson() const;
private:
    QString user, pass;
};

/***** NetCommander ******/
class NetCommander : public Eventful
{
    Q_OBJECT
public:
    NetCommander(NetTransport * transport, QObject *parent = nullptr);
    virtual ~NetCommander(){}

public slots:
//    virtual void start() = 0;
//    virtual void stop() = 0;
    virtual void netEventOut(QSharedPointer<Event>) = 0;

    void setAuthData(const AuthData & auth);
    // void makeAnswer();
    // MsgID whatAnswerID();

protected:
    QSharedPointer<NetProtocol> proto;
    QSharedPointer<NetTransport> phy;
    AuthData auth;

signals:
    qint32 send(QByteArray data);
protected slots:
    virtual void recv(QByteArray data) = 0;

};



/******************** Implementation *********************/

/* need to set:
 *  *NetTransport
 *  **ip and port endpoint
 *  **
 */

/***** NetCommanders ******/

class EventClient : public NetCommander
{
    virtual void run();
};

class PollingClient : public NetCommander
{
    virtual void run();
};

class RpcClient : public NetCommander
{
    virtual void run();
};

#endif // NETWORKCLIENT_H
