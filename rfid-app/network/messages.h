#ifndef MESSAGES_H
#define MESSAGES_H

#include "types.h"


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
    const MsgID msgid;

};

class NetworkProtocol
{
  public:
    enum ParseError{OK, ERR};

    virtual QSharedPointer<NetworkMessage> parse(QByteArray data, ParseError & err)=0;
    virtual QByteArray pack(QSharedPointer<NetworkMessage> data)=0;
};


/************ msg ************/

class TagEventMsg : public NetworkMessage
{

};

class ErrEventMsg : public NetworkMessage
{

};

class AnswertMsg : public NetworkMessage
{

};

class RequestMsg : public NetworkMessage
{

};

class ScannerRuntimeMsg : public NetworkMessage
{

};

class EventsCountMsg : public NetworkMessage
{

};

/************ prt ************/





#endif // MESSAGES_H
