#include "events.h"
#include <Qt>


/***************** Event ******************/

QString Event::toString() const
{
    return QStringLiteral("[") + time.toString(Qt::ISODate) + "]: ";
}


/*************** Tag Event ****************/

QString TagEvent::toString() const
{
    QString eventName;
    switch (tagevent) {
    case ENTER: eventName = "enter to zone"; break;
    case LEAVE: eventName = "leave from zone"; break;
    default: eventName = "undefined tag event"; break;
    }
    return Event::toString() + "Tag " + tag->toString() + " " + eventName;
}

QJsonObject TagEvent::toJson() const
{
    QJsonObject json;
    json["event"] = tagevent;
    json["time"] = time.toString(Qt::ISODate);
    json["tagid"] = tag->toString();
    return json;
}


/*************** Inf Event ****************/

QString InfoEvent::toString() const
{
    QString rangeStr;
    switch (level) {
    case EventLevel::DEBUG: rangeStr = "DEBUG"; break;
    case EventLevel::INFO: rangeStr = "INFO"; break;
    case EventLevel::WARNING: rangeStr = "WARNING"; break;
    case EventLevel::ERROR: rangeStr = "ERROR"; break;
    case EventLevel::CRITICAL: rangeStr = "CRITICAL"; break;
    }

    QString placeStr;
    switch (place) {
    case EventPlace::SCANNER: placeStr = "SCANNER"; break;
    case EventPlace::NETWORK: placeStr = "NETWORK"; break;
    case EventPlace::SYSTEM: placeStr = "SYSTEM"; break;
    case EventPlace::COMMANDER: placeStr = "COMMANDER"; break;
    }

    return Event::toString() + placeStr + "->" + rangeStr + "->ID:" + QString::number(eventid) +
            "  \"" + description + "\"";
}

QJsonObject InfoEvent::toJson() const
{
    QJsonObject json;
    json["time"] = time.toString(Qt::ISODate);
    json["level"] = level;
    json["place"] = place;
    json["errorid"] = eventid;
    json["description"] = description;
    return json;
}


/************** Eventiable **************/

void Eventful::connectAsEventDrain(Eventful *obj)
{
    connect(this, SIGNAL(sysEvent(QSharedPointer<Event>)),
            obj, SIGNAL(sysEvent(QSharedPointer<Event>)));
}

void Eventful::disconnectAsEventDrain(Eventful *obj)
{
    disconnect(this, SIGNAL(sysEvent(QSharedPointer<Event>)),
               obj, SIGNAL(sysEvent(QSharedPointer<Event>)));
}

void Eventful::connectAsEventSource(Eventful *obj)
{
    connect(obj, SIGNAL(sysEvent(QSharedPointer<Event>)),
            this, SIGNAL(sysEvent(QSharedPointer<Event>)));
}

void Eventful::disconnectAsEventSource(Eventful *obj)
{
    disconnect(obj, SIGNAL(sysEvent(QSharedPointer<Event>)),
               this, SIGNAL(sysEvent(QSharedPointer<Event>)));
}
