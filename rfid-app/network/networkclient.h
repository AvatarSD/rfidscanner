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
Q_DECLARE_METATYPE(NetCommanderState::States)
Q_DECLARE_METATYPE(NetCommanderState)

/****** NetCommander *******/
class NetCommander : public Eventful
{
    Q_OBJECT
public:
    enum WorkMode{
        DISABLED, POOL, EVENT
    };
    NetCommander(NetTransport *transport,
                 NetProtocol *protocol,
                 QObject *parent = nullptr);
    virtual ~NetCommander();
    /* settings */
    virtual NetPoint getAddr() const = 0;
    virtual void setAddr(const NetPoint &value) = 0;
    virtual WorkMode getMode() const = 0;
    virtual void setMode(const WorkMode &value) = 0;
    virtual QAuthenticator getAuth() const = 0;
    virtual void setAuth(const QAuthenticator &value) = 0;
    /* timing options */
    virtual uint getMsgTransmitRepeatSec() const = 0;
    virtual void setMsgTransmitRepeatSec(uint value) = 0;
    virtual uint getMsgMaxAtemptToDelete() const = 0;
    virtual void setMsgMaxAtemptToDelete(uint value) = 0;
    virtual int getMsgInspectPeriodMsec() const = 0;
    virtual void setMsgInspectPeriodMsec(int value) = 0;
public slots:
    virtual void netEventIn(QSharedPointer<Event>) = 0;
    virtual void start() = 0; // connect to server
    virtual void stop() = 0;  // disconnect from server
    const NetCommanderState &getState() const;
signals:
    void stateChanged(const NetCommanderState &state);
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

#define MSG_INSPECT_PERIOD_MSEC 200
#define MSG_TRANSMIT_REPEAT_SEC 5
#define MSG_TRANSMIT_DELETE_NUM 5
/****** NetCommanders ******/
class BasicV1Client : public NetCommander
{
    Q_OBJECT
public:
    BasicV1Client(NetTransport* transport,
                  NetProtocol* protocol,
                  const NetPoint &addr,
                  QObject *parent = nullptr);
    virtual ~BasicV1Client()
    {}
    /* settings */
    virtual NetPoint getAddr() const;
    virtual void setAddr(const NetPoint &value);
    virtual WorkMode getMode() const;
    virtual void setMode(const WorkMode &value);
    virtual QAuthenticator getAuth() const;
    virtual void setAuth(const QAuthenticator &value);
    /* timing options */
    virtual uint getMsgTransmitRepeatSec() const;
    virtual void setMsgTransmitRepeatSec(uint value);
    virtual uint getMsgMaxAtemptToDelete() const;
    virtual void setMsgMaxAtemptToDelete(uint value);
    virtual int getMsgInspectPeriodMsec() const;
    virtual void setMsgInspectPeriodMsec(int value);
public slots:
    virtual void netEventIn(QSharedPointer<Event> event);
    virtual void start();
    virtual void stop();
protected slots:
    virtual void receiveMsg(QByteArray data);
    void stateChanged(NetCommanderState state);
protected:
    QQueue<QSharedPointer<NetMessage>> messageQueue;
    NetPoint addr;
    WorkMode mode;
    QAuthenticator auth;
private slots:
    void sendMsgEnqueue(QSharedPointer<NetMessage> msg);
    void sendMsgDirect(QSharedPointer<NetMessage> msg);
    // void makeAnswer();
    // MsgID whatAnswerID();
    void msgInspect();
private:
    QTimer inspectTimer;
    uint msgTransmitRepeatSec;
    uint msgMaxAtemptToDelete;
};


#endif // NETWORKCLIENT_H
