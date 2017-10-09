#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include "transportiface.h"
#include <QObject>
#include <types.h>
#include <QQueue>
#include <QAuthenticator>


/********************** Interface ************************/

/**************** Level 4 *****************/

/**** NetCommanderState ****/
class NetCommanderState{
public:
    enum States{
        DISCONNECTED,
        LOOKUPING,
        CONNECTING,
        CONNECTED,
        CLOSING,
        AUTHENTICATED
    };
    NetCommanderState(){}


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
    NetCommander(NetTransport *transport,
                 NetProtocol *protocol,
                 QObject *parent = nullptr);
    virtual ~NetCommander();
signals:
    void stateChanged(NetCommanderState state);
public slots:
    virtual void netEventOut(QSharedPointer<Event>) = 0;
    virtual void start() = 0; // connect to server
    virtual void stop() = 0;  // disconnect from server
    const NetCommanderState &getState() const;
/****************************/
protected:
    QScopedPointer<NetTransport> phy;
    QScopedPointer<NetProtocol> proto;
signals:
    qint32 send(QByteArray data);
    void connectToHost(const NetPoint &addr);
    void disconnectFromHost();
protected slots:
    virtual void recv(QByteArray data) = 0;
    void setState(NetCommanderState state);
/****************************/
private:
    QThread phyThread;
    NetCommanderState state;
private slots:
    void transportStateChanged(NetState newState);
};



/******************** Implementation *********************/

/**************** Level 4 *****************/

/****** NetCommanders ******/
class BasicV1Client : public NetCommander
{
    Q_OBJECT
public:
    enum WorkMode{
        POOL, EVENT
    };
    BasicV1Client(NetTransport* transport,
                  NetProtocol* protocol,
                  const NetPoint &addr,
                  QObject *parent = nullptr);
    virtual ~BasicV1Client()
    {}
    NetPoint getAddr() const;
    void setAddr(const NetPoint &value);
    WorkMode getMode() const;
    void setMode(const WorkMode &value);
    QAuthenticator getAuth() const;
    void setAuth(const QAuthenticator &value);
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
    NetPoint addr;
    WorkMode mode;
    QAuthenticator auth;
};


#endif // NETWORKCLIENT_H
