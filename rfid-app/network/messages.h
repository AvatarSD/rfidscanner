#ifndef MESSAGES_H
#define MESSAGES_H

#include "events.h"
#include "common/scannerfacade.h"

#include <QUuid>
#include <QJsonObject>


/**************************************************/
/*********************** Face *********************/

/************ gen ************/
class NetworkMessage
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

    NetworkMessage(MsgID id, QJsonObject payload); // autogenerate UUID
    NetworkMessage(MsgID id, QJsonObject payload, QUuid uuid);

    static QSharedPointer<NetworkMessage> parse(QByteArray data, ParseError & err);
    QByteArray pack();
    virtual void execute(ScannerFacade*){}

    /***** data *****/
    const QUuid uuid;
    const MsgID msgid;
    const QJsonObject payload;
};

/************ prot ************/
class NetProtocol : public Eventful
{
public:
};



/**************************************************/
/********************** Impl **********************/

/************ msgs ************/
/*class TagEventMsg : public NetworkMessage
{
public:
    TagEventMsg(QJsonObject payload) :
        NetworkMessage(TAG_EVENT, payload) {}
    virtual ~TagEventMsg(){}
};
class ErrEventMsg : public NetworkMessage
{
public:
    ErrEventMsg(QJsonObject payload) :
        NetworkMessage(ERROR_EVENT, payload) {}
    virtual ~ErrEventMsg(){}
};
class AnswertMsg : public NetworkMessage
{
public:
    AnswertMsg(QJsonObject payload) :
        NetworkMessage(ANSWER, payload) {}
    virtual ~AnswertMsg(){}
};
class RequestMsg : public NetworkMessage
{
public:
    RequestMsg(QJsonObject payload) :
        NetworkMessage(REQUEST, payload) {}
    virtual ~RequestMsg(){}
};
class ScannerRuntimeMsg : public NetworkMessage
{
public:
    ScannerRuntimeMsg(QJsonObject payload) :
        NetworkMessage(SCANNER_RUNTIME, payload) {}
    virtual ~ScannerRuntimeMsg(){}
};
class EventsCountMsg : public NetworkMessage
{
public:
    EventsCountMsg(QJsonObject payload) :
        NetworkMessage(EVENT_COUNT, payload) {}
    virtual ~EventsCountMsg(){}
};*/


/************ prt ************/





#endif // MESSAGES_H
