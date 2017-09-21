#include "events.h"
#include <Qt>


/***************** Event ******************/

QString Event::toString() const
{
    QString type;
    switch (this->event) {
    case EventType::INF: type = "INF"; break;
    case EventType::TAG: type = "TAG"; break;
    default: type = "UNK"; break;
    }
    return type + " [" + time.toString(Qt::ISODate) + "]: ";
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
    return Event::toString() + tag->toString() + " " + eventName;
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
    case EventLevel::DEBUG: rangeStr = "DEBUG";
    case EventLevel::INFO: rangeStr = "INFO";
    case EventLevel::WARNING: rangeStr = "WARNING";
    case EventLevel::ERROR: rangeStr = "ERROR";
    case EventLevel::CRITICAL: rangeStr = "CRITICAL";
    }

    QString placeStr;
    switch (place) {
    case EventPlace::SCANNER: placeStr = "SCANNER";
    case EventPlace::NETWORK: placeStr = "NETWORK";
    case EventPlace::SYSTEM: placeStr = "SYSTEM";
    case EventPlace::COMMANDER: placeStr = "COMMANDER";
    }

    return Event::toString() + rangeStr + "->" + placeStr + "->ID:" + eventid +
            " \"" + description + "\"";
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
