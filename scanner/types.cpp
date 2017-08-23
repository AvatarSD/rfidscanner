#include "types.h"
#include <cstring>


/*********** TagID ***********/

TagID::TagID(const TagID &id)
{
    memcpy(this->id, id.id, TID_LENGHT);
}

QString TagID::toString() const
{
    QString str;
    for(uint i = 0; i < TID_LENGHT; i++){
        str += id[i];
        if(i!=TID_LENGHT-1)
            str += ":";
    }
    return str;
}

/************ Tag ************/

void Tag::writeToJson(QJsonObject &json) const
{
    json["tagid"] = tid.toString();
}

/********** LogEvent *********/

LogEvent::LogEvent(const Tag &id, LogEvent::EventType evtype) :
    tag(id), event(evtype), time(QDateTime::currentDateTimeUtc())
{
    //time(QDateTime::currentDateTimeUtc());
}

void LogEvent::writeToJson(QJsonObject &json) const
{
    QJsonObject tagjson;
    tag.writeToJson(tagjson);
    json["tag"] = tagjson;
    json["event"] = getEvent();
    json["timestamp"] = getMSecsSinceEpoch();
}

Tag LogEvent::getTag() const
{
    return tag;
}

LogEvent::EventType LogEvent::getEvent() const
{
    return event;
}

qint64 LogEvent::getMSecsSinceEpoch() const
{
    return time.toMSecsSinceEpoch();
}

//QString LogEvent::readToString()
//{
//}

