#include "types.h"


/*********** TagID ***********/

QString TagID::toString()
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

void Tag::writeToJson(QJsonObject &json)
{
    json["tagid"] = tid.toString();
}

/********** LogEvent *********/

void LogEvent::writeToJson(QJsonObject &json)
{
    QJsonObject tagjson;
    tag.writeToJson(tagjson);
    json["tag"] = tagjson;
    json["event"] = event;
    json["timestamp"] = timestamp;
}

Tag LogEvent::getTag() const
{
    return tag;
}

EventType LogEvent::getEvent() const
{
    return event;
}

uint64_t LogEvent::getTimestamp() const
{
    return timestamp;
}

//QString LogEvent::readToString()
//{
//}

