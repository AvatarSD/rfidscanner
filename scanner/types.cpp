#include "types.h"
#include <cstring>


/*********** Tag Memory Banks ***********/

TagID::TagID(const TagID &id)
{
    memcpy(this->id, id.id, TID_LENGHT);
}


TagMemory::TagMemoryBank TagID::getMemoryBankNumber() const
{return TagMemory::TagMemoryBank::TID;}

TagMemory::TagMemoryBank EPCMem::getMemoryBankNumber() const
{return TagMemory::TagMemoryBank::EPC;}

TagMemory::TagMemoryBank User::getMemoryBankNumber() const
{return TagMemory::TagMemoryBank::USEER;}

TagMemory::TagMemoryBank Reserve::getMemoryBankNumber() const
{return TagMemory::TagMemoryBank::RESERVE;}


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

QString EPCMem::toString() const
{
    return "";
}

QString User::toString() const
{
    return "";
}

QString Reserve::toString() const
{
    return "";
}

/************ Tag ************/

Tag::Tag(const TagID &tid, const EPCMem &epc,
         const User &user, const Reserve &reserve) :
    tid(tid), epc(epc), user(user), reserve(reserve)
{}

void Tag::writeToJson(QJsonObject &json) const
{
    json["tagid"] = tid.toString();
    json["epc"] = epc.toString();
    json["user"] = user.toString();
    json["reserve"] = reserve.toString();
}

const TagID &Tag::getTagID() const
{
    return tid;
}

const EPCMem &Tag::getEpc() const
{
    return epc;
}

const User &Tag::getUser() const
{
    return user;
}

const Reserve &Tag::getReserve() const
{
    return reserve;
}

void Tag::setEpc(const EPCMem &value)
{
    epc = value;
}

void Tag::setUser(const User &value)
{
    user = value;
}

void Tag::setReserve(const Reserve &value)
{
    reserve = value;
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


