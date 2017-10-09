#include "types.h"
#include <cstring>
#include <stdio.h>


/*********** Tag Memory Banks ***********/

TagID::TagID() : TagMemory(TID) {}
TagID::TagID(const TagID &val) : TagID() {*this = val;}

const TagID &TagID::operator =(const TagID &val){
    memcpy(this->id, val.id, TID_LENGHT);
    return *this;
}

bool TagID::operator ==(const TagID &val)
{
    for(uint i = 0; i < TID_LENGHT; i++)
        if(id[i] != val.id[i])
            return false;
    return true;
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

int TagID::fromString(const QString &tag)
{
    return sscanf(tag.toUtf8().data(),
                  "%X:%X:%X:%X:%X:%X:%X:%X",
                  (uint*)(id+0), (uint*)(id+1),(uint*)(id+2), (uint*)(id+3),
                  (uint*)(id+4), (uint*)(id+5),(uint*)(id+6), (uint*)(id+7));
}

/*EPCMem::EPCMem() : TagMemory(EPC) {}
User::User() : TagMemory(USER) {}
Reserve::Reserve() : TagMemory(RESERVE) {}


EPCMem::EPCMem(const EPCMem &val) : EPCMem() {*this = val;}
User::User(const User &val) : User() {*this = val;}
Reserve::Reserve(const Reserve &val) : Reserve() {*this = val;}
*/

/*const EPCMem &EPCMem::operator =(const EPCMem &val){
    return *this;}

const User &User::operator =(const User &val){
    return *this;}

const Reserve &Reserve::operator =(const Reserve &val){
    return *this;}
*/
/*QString EPCMem::toString() const
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
*/

/************ Tag ************/
/*
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
*/



