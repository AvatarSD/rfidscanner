#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <types.h>
#include <QQueue>
#include <QAuthenticator>
#include <QMutex>
#include "b_protocol.h"


/********************** Level 4(D) ***********************/

/*************** Interface ****************/

/****** NetClientState *****/
class NetClientState{
public:
    /* static */
    enum NetClientStateEnum {
        DISCONNECTED,
        LOOKUPING,
        CONNECTING,
        CONNECTED,
        CLOSING,
        AUTHENTICATED };
    static NetClientStateEnum fromPhyStateHelper(QAbstractSocket::SocketState state);
    /* set */
    NetClientState() : state(DISCONNECTED){}
    void fromPhyState(const NetPhyState &state);
    void fromRawState(NetClientStateEnum state, QString msg);
    /* get */
    NetClientStateEnum stateEnum() const;
    QString stateMessage() const;
    /* helpers */
    bool operator ==(NetClientStateEnum state) const;
    bool operator !=(NetClientStateEnum state) const{
        return !this->operator ==(state);}
private:
    NetClientStateEnum state;
    QString msg;
    mutable QMutex access;
};
Q_DECLARE_METATYPE(NetClientState::NetClientStateEnum)
Q_DECLARE_METATYPE(NetClientState*)
Q_DECLARE_METATYPE(QAuthenticator)

/******** NetClient ********/
class NetClient : public Eventful
{
    Q_OBJECT

public:
    enum WorkMode{
        DISABLED, POOL, EVENT
    };
    Q_ENUM(WorkMode)
    NetClient(NetPhy *transport,
                 NetProtocol *protocol,
                 QObject *parent = nullptr);
    virtual ~NetClient();
public slots:
    /* settings */
    virtual NetPoint getAddr() const = 0;
    virtual void setAddr(const NetPoint &value) = 0;
    virtual QAuthenticator getAuth() const = 0;
    virtual void setAuth(const QAuthenticator &value) = 0;
    virtual WorkMode getMode() const = 0;
    virtual void setMode(WorkMode value) = 0;
    /* timing options */
    virtual uint getMsgTransmitRepeatSec() const = 0;
    virtual void setMsgTransmitRepeatSec(uint value) = 0;
    virtual uint getMsgMaxAtemptToDelete() const = 0;
    virtual void setMsgMaxAtemptToDelete(uint value) = 0;
    virtual int getMsgInspectPeriodMsec() const = 0;
    virtual void setMsgInspectPeriodMsec(int value) = 0;
    /* main event input*/
    virtual void netEventIn(QSharedPointer<Event>) = 0;
    /* control */
    virtual void start() = 0; // connect to server
    virtual void stop() = 0;  // disconnect from server
    /* info */
    const NetClientState *state() const;
signals:
    void stateChanged(const NetClientState *state);
    /****************************/
protected slots:
    virtual void receiveMsg(QByteArray data) = 0;
    void transmitMsg(QByteArray msg);
signals:
    void connectToHost(const NetPoint &addr);
    void disconnectFromHost();
    /****************************/
private:
    QScopedPointer<NetPhy> phy;
    QScopedPointer<NetProtocol> proto;
    QThread phyThread;
    NetClientState m_state;
private slots:
    void recv(QByteArray data);
    void NetPhyStateHandler(NetPhyState netPhyState);
signals:
    qint32 send(QByteArray data);
};


/************ Implementation **************/

/********* Supply **********/
#define MSG_INSPECT_PERIOD_MSEC 200
#define MSG_TRANSMIT_REPEAT_SEC 5
#define MSG_TRANSMIT_DELETE_NUM 5

/***** NetClientV1Basic ****/
class NetClientV1Basic : public NetClient
{
    Q_OBJECT
public:
    NetClientV1Basic(NetPhy* transport,
                  NetProtocol* protocol,
                  QObject *parent = nullptr);
    virtual ~NetClientV1Basic(){}
public slots:
    /* settings */
    virtual NetPoint getAddr() const;
    virtual void setAddr(const NetPoint &value);
    virtual QAuthenticator getAuth() const;
    virtual void setAuth(const QAuthenticator &value);
    virtual WorkMode getMode() const;
    virtual void setMode(WorkMode value);
    /* timing optio */
    virtual uint getMsgTransmitRepeatSec() const;
    virtual void setMsgTransmitRepeatSec(uint value);
    virtual uint getMsgMaxAtemptToDelete() const;
    virtual void setMsgMaxAtemptToDelete(uint value);
    virtual int getMsgInspectPeriodMsec() const;
    virtual void setMsgInspectPeriodMsec(int value);
    /* main event input*/
    virtual void netEventIn(QSharedPointer<Event>);
    /* control */
    virtual void start(); // connect to server
    virtual void stop();  // disconnect from server
protected slots:
    virtual void receiveMsg(QByteArray data);
    void netClientStateChangedHelperHandler(const NetClientState *state);
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
