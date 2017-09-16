#include "events.h"
#include <Qt>


/***************** Event ******************/

QString Event::toString() const
{
    QString type;
    switch (this->event) {
    case EventType::INF: type += "INF"; break;
    case EventType::TAG: type += "TAG"; break;
    default: type = "UNK"; break;
    }
    return type + " [" + time.toString(Qt::ISODate) + "]: ";
}


/*************** Tag Event ****************/

QString TagEvent::toString() const
{
    QString eventName;
    switch (tagevent) {
    case ENTER: eventName += "enter to zone"; break;
    case LEAVE: eventName += "leave from zone"; break;
    default: eventName += "undefined event"; break;
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

}


/************ Inf Event Types *************/
