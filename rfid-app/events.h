#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"
#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonParseError>
//#include <inttypes.h>
#include <QSharedPointer>



/***************** Event ******************/
class Event
{
public:
    enum EventType{INF, TAG};
    Event(EventType event, QDateTime time = QDateTime::currentDateTime()):
        event(event), time(time) {}
    virtual ~Event(){}
    virtual QString toString() const;
    virtual QJsonObject toJson() const = 0;
    /*******************************/
    const EventType event;
    const QDateTime time;
};
Q_DECLARE_METATYPE(QSharedPointer<Event>)

/************ Tag Event Types *************/
/* base */
class TagEvent : public Event
{
public:
    /* required to protocol */
    enum TagEventType{
        LEAVE = 1,
        ENTER = 2};
    TagEvent(QString tag, TagEventType evtype, 
             QDateTime time = QDateTime::currentDateTime()) :
        Event(TAG, time), tagevent(evtype), tag(tag) {}
    virtual ~TagEvent(){}
    virtual QString toString() const;
    virtual QJsonObject toJson() const;
    const TagEventType tagevent;
private:
    QString tag;
};

/* extended */
class TagEnterEvent : public TagEvent
{
public:
    TagEnterEvent(QString tag, QDateTime time = QDateTime::currentDateTime()) :
        TagEvent(tag, TagEventType::ENTER, time){}
    virtual ~TagEnterEvent(){}
};

class TagLeaveEvent : public TagEvent
{
public:
    TagLeaveEvent(QString tag, QDateTime time = QDateTime::currentDateTime()) :
        TagEvent(tag, TagEventType::LEAVE, time){}
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
        PHY,
        PARSER,
        SCANNER,
        COMMANDER,
        MANENGER
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
        COMMANDER,
        AUTHENTICATOR,
        MESSAGE_PARSER
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
        LOGFILE_OPEN_ERR,
        FACADE_STATUS
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
