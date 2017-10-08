#ifndef MESSAGES_H
#define MESSAGES_H

#include "events.h"
#include "common/scannerfacade.h"

#include <QUuid>
#include <QJsonObject>


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
    enum ParseError{OK, ERR};

    NetMessage(MsgID id, QJsonObject payload); // autogenerate UUID
    NetMessage(MsgID id, QJsonObject payload, QUuid uuid);

    QByteArray pack();
    static QSharedPointer<NetMessage> parse(QByteArray data, ParseError & err);
    virtual void execute(ScannerFacade*){}

    /***** data *****/
    const QUuid uuid;
    const MsgID msgid;
    const QJsonObject payload;
};



/**************************************************/
/********************** Impl **********************/

/************ msgs ************/
/*class TagEventMsg : public NetMessage
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
};*/


/************ prt ************/





#endif // MESSAGES_H
