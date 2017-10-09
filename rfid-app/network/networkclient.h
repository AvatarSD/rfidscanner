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
    NetCommanderState() : state(DISCONNECTED){}
    NetCommanderState(States state, QString msg) :
        state(state), msg(msg){}
    NetCommanderState(const NetState & state);
    void fromNetState(const NetState &state);
    void fromRawState(States state, QString msg);
    void fromSelfState(const NetCommanderState &state);
    States getState() const;
    QString getMsg() const;
    bool operator ==(States state);
    static States fromSocketState(QAbstractSocket::SocketState state);
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
public slots:
    virtual void netEventOut(QSharedPointer<Event>) = 0;
    virtual void start() = 0; // connect to server
    virtual void stop() = 0;  // disconnect from server
    const NetCommanderState &getState() const;
signals:
    void stateChanged(NetCommanderState state);
    /****************************/
protected slots:
    virtual void receiveMsg(QByteArray data) = 0;
    void transmitMsg(QByteArray msg);
    void setState(const NetCommanderState &state);
signals:
    void connectToHost(const NetPoint &addr);
    void disconnectFromHost();
    /****************************/
private:
    QScopedPointer<NetTransport> phy;
    QScopedPointer<NetProtocol> proto;
    QThread phyThread;
    NetCommanderState state;
private slots:
    void recv(QByteArray data);
    void transportStateChanged(NetState newState);
signals:
    qint32 send(QByteArray data);
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
    virtual void netEventOut(QSharedPointer<Event> event);
    virtual void start();
    virtual void stop();
protected slots:
    virtual void receiveMsg(QByteArray data);
    void stateChanged(NetCommanderState state);
    // void makeAnswer();
    // MsgID whatAnswerID();
protected:
    QQueue<QSharedPointer<NetMessage>> messageQueue;
    NetPoint addr;
    WorkMode mode;
    QAuthenticator auth;
};


#endif // NETWORKCLIENT_H
