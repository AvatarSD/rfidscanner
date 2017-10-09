#ifndef MESSAGES_H
#define MESSAGES_H

#include "events.h"
#include "common/scannerfacade.h"

#include <QUuid>
#include <QJsonObject>
#include <QAuthenticator>
#include <QDateTime>


/**************************************************/
/*********************** Face *********************/

/************ gen ************/
class NetMessage
{
public:
    enum MsgID{
        TAG_EVENT,
        ERROR_EVENT,
        ANSWER,
        REQUEST,
        SCANNER_RUNTIME,
        EVENT_COUNT/*,
        SCANNER_SETTINGS,
        NET_STATUS,
        SYSTEM_SETTINGS,
        TAG_MEMORY,
        SAVED_TAG,
        USERMOD,
        USER_RUNTIME*/
    };
    struct ParseError{
        enum MsgParseStatus{
            OK = 0,
            AUTH_ERR = 1, // now only for server
            MSGID_ERR = 2,
            JSON_ERR = 3,
            CONTENT_ERR = 4
        };
        MsgParseStatus status;
        QJsonParseError jsonError;
    };

    /**** constructor ****/
    NetMessage(MsgID id, QJsonObject payload); // autogenerate UUID
    NetMessage(MsgID id, QJsonObject payload, QUuid uuid);
    virtual ~NetMessage(){}

    /**** serialaized ****/
    QByteArray pack(const QAuthenticator& auth);

    /**** helper ****/
    static QJsonObject authDataToJson(const QAuthenticator &auth);

    /**** factory ****/
    static QSharedPointer<NetMessage> createMsg(MsgID id, QJsonObject payload, QUuid uuid);
    static QSharedPointer<NetMessage> parse(QByteArray data, ParseError * err);

    /**** for ruture ****/
    virtual void execute(ScannerFacade*){}

    /***** data *****/
    const MsgID msgid;
    const QJsonObject payload;
    const QUuid uuid;

    /**** status ****/
    uint getTransmitCount() const;
    QDateTime getLastTransmit() const;
private:
    uint transmitCount;
    QDateTime lastTransmit;

};



/************************* Impl **************************/

/********** Msgs ***********/
class TagEventMsg : public NetMessage
{
public:
    TagEventMsg(QJsonObject payload) :
        NetMessage(TAG_EVENT, payload) {}
    virtual ~TagEventMsg(){}
};
class ErrEventMsg : public NetMessage
{
public:
    ErrEventMsg(QJsonObject payload) :
        NetMessage(ERROR_EVENT, payload) {}
    virtual ~ErrEventMsg(){}
};
class AnswertMsg : public NetMessage
{
public:
    AnswertMsg(QJsonObject payload) :
        NetMessage(ANSWER, payload) {}
    virtual ~AnswertMsg(){}
};
class RequestMsg : public NetMessage
{
public:
    RequestMsg(QJsonObject payload) :
        NetMessage(REQUEST, payload) {}
    virtual ~RequestMsg(){}
};
class ScannerRuntimeMsg : public NetMessage
{
public:
    ScannerRuntimeMsg(QJsonObject payload) :
        NetMessage(SCANNER_RUNTIME, payload) {}
    virtual ~ScannerRuntimeMsg(){}
};
class EventsCountMsg : public NetMessage
{
public:
    EventsCountMsg(QJsonObject payload) :
        NetMessage(EVENT_COUNT, payload) {}
    virtual ~EventsCountMsg(){}
};


/************ prt ************/





#endif // MESSAGES_H
