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

/**** NetCommanderState ****/
class NetCommanderState{
public:
    enum States{
        DISCONNECTED, CONNECTED, AUTHENTICATED
    };

private:
    States state;
    QString msg;
};
Q_DECLARE_METATYPE(NetCommanderState)

/****** NetCommander *******/
class NetCommander : public Eventful
{
    Q_OBJECT
public:
    enum WorkMode{
        POOL, EVENT
    };
    NetCommander(NetTransport *transport,
                 NetProtocol *protocol,
                 const NetPoint &addr,
                 QObject *parent = nullptr);
    virtual ~NetCommander();
    AuthData getAuth() const;
    void setAuth(const AuthData &value);
    WorkMode getMode() const;
    void setMode(WorkMode mode);
    NetPoint getAddr() const;
    void setAddr(const NetPoint &value);
signals:
    void stateChanged(NetCommanderState state);
public slots:
    virtual void netEventOut(QSharedPointer<Event>) = 0;
    virtual void start() = 0; // connect to server
    virtual void stop() = 0;  // disconnect from server
protected:
    void setState(NetCommanderState state);
    NetCommanderState getState() const;
    /****************************/
    QSharedPointer<NetTransport> phy;
    QSharedPointer<NetProtocol> proto;
    NetPoint addr;
    AuthData auth;
    WorkMode mode;
signals:
    qint32 send(QByteArray data);
protected slots:
    virtual void recv(QByteArray data) = 0;
private slots:
    void transportStateChanged(NetState newState);
private:
    QThread phyThread;
    NetCommanderState state;
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
                  const NetPoint &addr,
                  QObject *parent = nullptr);
    virtual ~BasicV1Client();
public slots:
    virtual void netEventOut(QSharedPointer<Event>);
    virtual void start();
    virtual void stop();
protected slots:
    virtual void recv(QByteArray data);
    // void makeAnswer();
    // MsgID whatAnswerID();
protected:
    QQueue<QSharedPointer<NetMessage>> messageQueue;
};


#endif // NETWORKCLIENT_H
