#ifndef TRANSPORTIFACE_H
#define TRANSPORTIFACE_H

#include <QString>
#include <QAbstractSocket>
#include <QScopedPointer>
#include <QTimer>
#include "events.h"


/********************** Level 1(A) ***********************/

/*************** Interface ****************/

/******** NetPoint *********/
class NetPoint{
public:
    NetPoint();
    NetPoint(QString addr, quint16 port);
    QString addr() const;
    quint16 port() const;
    void addr(QString addr);
    void port(quint16 port);
    bool isNull() const;
    void reset();
private:
    QString _addr;
    quint16 _port;
};
Q_DECLARE_METATYPE(NetPoint)

/******* NetPhyState *******/
class NetPhyState
{
public:
    NetPhyState() = default;
    NetPhyState(const NetPhyState&) = default;
    /**************************/
    NetPhyState(QAbstractSocket::SocketState state, QString message);
    QString toString() const;
    bool operator == (QAbstractSocket::SocketState state);
    QAbstractSocket::SocketState getState() const;
private:
    QString toRawString() const;
    QAbstractSocket::SocketState state;
    QString msg;
};
Q_DECLARE_METATYPE(NetPhyState)

/********* NetPhy **********/
class NetPhy : public Eventful
{
    Q_OBJECT
public:
    NetPhy(QObject* parent = nullptr) :
        Eventful(parent) {qRegisterMetaType<NetPhyState>();}
    virtual ~NetPhy(){}
signals:
    void recv(QByteArray data);
    void stateChanged(NetPhyState newState);
public slots:
    virtual void connectToHost(const NetPoint &addr = NetPoint()) = 0;
    virtual void disconnectFromHost() = 0;
    virtual qint32 send(QByteArray data) = 0;
    /****************************/
    virtual NetPhyState state() const = 0;
};


/************ Implementation **************/

/******** NetPhyTcp ********/
class NetPhyTcp : public NetPhy
{
    Q_OBJECT
public:
    NetPhyTcp(QObject* parent = nullptr);
    NetPhyTcp(QAbstractSocket * socket, QObject* parent = nullptr);
    ~NetPhyTcp();
public slots:
    virtual void connectToHost(const NetPoint& addr = NetPoint());
    virtual void disconnectFromHost();
    virtual qint32 send(QByteArray data);
    /****************************/
    virtual NetPhyState state() const;
protected slots:
    void socketReadyRead();
    void run();
    void socketStateChanged(QAbstractSocket::SocketState state);
    void socketError(QAbstractSocket::SocketError error);
protected:
    static QString stateToString(QAbstractSocket::SocketState state,
                                 const NetPoint &host);
    QScopedPointer<QAbstractSocket> socket;
    QTimer zerotimer;
    NetPoint host;
};


#endif // TRANSPORTIFACE_H
