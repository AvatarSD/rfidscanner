#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"
#include <QString>
#include <QDateTime>


/***************** Event ******************/
class Event : public Serialaizeable
{
public:
    enum EventType{INF, TAG};
    Event(EventType event) : event(event),
        time(QDateTime::currentDateTimeUtc()) {}
    virtual ~Event(){}
    virtual QString toString() const;
    const EventType event;
    const QDateTime time;
};


/************ Tag Event Types *************/
/* base */
class TagEvent : public Event
{
public:
    /* required to protocol */
    enum TagEventType{
        LEAVE = 1,
        ENTER = 2};
    TagEvent(QSharedPointer<TagID> tag, TagEventType evtype) :
        Event(TAG), tagevent(evtype), tag(tag) {}
    virtual ~TagEvent(){}
    const TagID & getTag() const {return *tag;}
    const TagEventType tagevent;
    // Serialaizeable interface
    virtual QString toString() const;
    virtual QJsonObject toJson() const;
private:
    QSharedPointer<TagID> tag;
};

/* extended */
class TagEnterEvent : public TagEvent
{
public:
    TagEnterEvent(QSharedPointer<TagID> tag) :
        TagEvent(tag, TagEventType::ENTER){}
    virtual ~TagEnterEvent(){}
};

class TagLeaveEvent : public TagEvent
{
public:
    TagLeaveEvent(QSharedPointer<TagID> tag) :
        TagEvent(tag, TagEventType::LEAVE){}
    virtual ~TagLeaveEvent(){}
};


/************ Inf Event Types *************/
/* baisic, do not use */
class InfoEvent : public Event
{
public:
    enum EventRange{
        DEBUG = 1,
        INFO = 2,
        WARNING = 3,
        ERROR = 4,
        CRITICAL = 5
    };
    enum EventPlace{
        SCANNER = 1,
        NETWORK = 2,
        SYSTEM = 3,
        COMMANDER = 4
    };

    InfoEvent(EventRange range, EventPlace place, QString dscr) :
        Event(INF), range(range), place(place), description(dscr) {}
    virtual ~InfoEvent(){}

    const EventRange range;
    const EventPlace place;
    const QString description;

    // Serialaizeable interface
    virtual QString toString() const;
    virtual QJsonObject toJson() const;
};

/* usable classes */
class ScannerEvent : public InfoEvent
{
public:
    enum ErrorType{
        OK = 0,
        ConnectionBreak = 1
    };
    ScannerEvent(EventRange range, QString dscr, ErrorType error) :
        InfoEvent(range, SCANNER, dscr), error(error) {}
    virtual ~ScannerEvent(){}
    const ErrorType error;
    // Serialaizeable interface
    virtual QString toString() const;
    virtual QJsonObject toJson() const;
};

class NetworkEvent : public InfoEvent
{
public:
    enum ErrorType{
        OK = 0,
        ConnectionBreak = 1
    };
    NetworkEvent(EventRange range, QString dscr, ErrorType error) :
        InfoEvent(range, NETWORK, dscr), error(error) {}
    virtual ~NetworkEvent(){}
    const ErrorType error;
    // Serialaizeable interface
    virtual QString toString() const;
    virtual QJsonObject toJson() const;
};

class SystemEvent : public InfoEvent
{
public:
    enum ErrorType{
        OK = 0,
        ConnectionBreak = 1
    };
    SystemEvent(EventRange range, QString dscr, ErrorType error) :
        InfoEvent(range, SYSTEM, dscr), error(error) {}
    virtual ~SystemEvent(){}
    const ErrorType error;
    // Serialaizeable interface
    virtual QString toString() const;
    virtual QJsonObject toJson() const;
};

class CommanderEvent : public InfoEvent
{
public:
    enum ErrorType{
        OK = 0,
        ConnectionBreak = 1
    };
    CommanderEvent(EventRange range, QString dscr, ErrorType error) :
        InfoEvent(range, COMMANDER, dscr), error(error) {}
    virtual ~CommanderEvent(){}
    const ErrorType error;
    // Serialaizeable interface
    virtual QString toString() const;
    virtual QJsonObject toJson() const;
};



#endif // EVENTS_H
