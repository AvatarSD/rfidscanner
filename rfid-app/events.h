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
    Event() : Event(INF) {}
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
    enum EventLevel{
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

    InfoEvent(EventLevel level, EventPlace place, QString dscr, int eventid) :
        Event(INF), level(level), place(place), description(dscr), eventid(eventid)
    {}
    virtual ~InfoEvent(){}

    const EventLevel level;
    const EventPlace place;
    const QString description;
    const int eventid;

    // Serialaizeable interface
    virtual QString toString() const;
    virtual QJsonObject toJson() const;
};

/* usable classes */
class ScannerEvent : public InfoEvent
{
public:
    enum IDs{
        OK = 0,
        ConnectionBreak = 1
    };
    ScannerEvent(EventLevel level, IDs eventid, QString dscr) :
        InfoEvent(level, EventPlace::SCANNER, dscr, eventid) {}
    virtual ~ScannerEvent(){}
};

class NetworkEvent : public InfoEvent
{
public:
    enum IDs{
        SOCKET_ERROR,
        SOCKET_STATE,
        PARSER_ERROR,
        COMMANDER_STATE
    };
    NetworkEvent(EventLevel level, IDs eventid, QString dscr) :
        InfoEvent(level, NETWORK, dscr, eventid) {}
    virtual ~NetworkEvent(){}
};

class SystemEvent : public InfoEvent
{
public:
    enum IDs{
        LOGGER_DYNAMIC_CAST_ERR,
        LOGFILE_OPEN_ERR
    };
    SystemEvent(EventLevel level, IDs eventid, QString dscr) :
        InfoEvent(level, SYSTEM, dscr, eventid) {}
    virtual ~SystemEvent(){}
};

class CommanderEvent : public InfoEvent
{
public:
    enum IDs{
        OK = 0,
        UNKNOW_CMD = 1
    };
    CommanderEvent(EventLevel level, IDs eventid, QString dscr) :
        InfoEvent(level, COMMANDER, dscr, eventid) {}
    virtual ~CommanderEvent(){}
};

// todo: impl event ID's !


/************** Eventiable **************/

class Eventful : public QObject
{
    Q_OBJECT
public:
    Eventful(QObject*parent=nullptr): QObject(parent){}
    virtual ~Eventful(){}
    void connectAsEventDrain(Eventful*obj);
    void disconnectAsEventDrain(Eventful*obj);
    void connectAsEventSource(Eventful*obj);
    void disconnectAsEventSource(Eventful*obj);
signals:
    void sysEvent(QSharedPointer<Event>);
};



#endif // EVENTS_H
