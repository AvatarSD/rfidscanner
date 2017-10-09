#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include "transportiface.h"
#include <QObject>
#include <types.h>
#include <QQueue>


/********************** Interface ************************/

/**************** Level 4 *****************/

/******** Auth Data ********/
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

/****** NetCommander *******/
class NetCommander : public Eventful
{
    Q_OBJECT
public:
    enum WorkMode{
        POOL, EVENT
    };
    enum NetCommanderState{
        DISCONNECTED, CONNECTED, AUTHENTICATED
    };
    NetCommander(NetTransport *transport,
                 NetProtocol *protocol,
                 NetPoint addr,
                 QObject *parent = nullptr);
    virtual ~NetCommander();
    AuthData getAuth() const;
    void setAuth(const AuthData &value);
    WorkMode getMode() const;
    void setMode(WorkMode mode);
signals:
    void authenticated();
public slots:
    virtual void netEventOut(QSharedPointer<Event>) = 0;
    virtual void start() = 0; // connect to server
    virtual void stop() = 0;  // disconnect from server
protected:
    QSharedPointer<NetTransport> phy;
    QSharedPointer<NetProtocol> proto;
    NetPoint addr;
    AuthData auth;
    WorkMode mode;
    NetCommanderState state;
signals:
    qint32 send(QByteArray data);
protected slots:
    virtual void recv(QByteArray data) = 0;
    void transportStateChanged(NetState newState);
    void setstate(NetCommanderState state);
    // void makeAnswer();
    // MsgID whatAnswerID();
private:
    QThread phyThread;
};



/******************** Implementation *********************/

/**************** Level 4 *****************/

/****** NetCommanders ******/
class BasicV1Client : public NetCommander
{
    Q_OBJECT
public:
    BasicV1Client(NetTransport* transport,
                  NetProtocol* protocol,
                  NetPoint addr,
                  QObject *parent = nullptr);
    virtual ~BasicV1Client();
public slots:
    virtual void netEventOut(QSharedPointer<Event>);
    virtual void start();
    virtual void stop();
protected slots:
    virtual void recv(QByteArray data);
protected:
    QQueue<QSharedPointer<NetMessage>> messageQueue;
};


#endif // NETWORKCLIENT_H
