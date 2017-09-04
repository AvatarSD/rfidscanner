#ifndef TYPES_H
#define TYPES_H

#include <inttypes.h>
#include <QJsonObject>
#include <QString>
#include <QDateTime>
#include <QJsonParseError>
#include <QSharedPointer>



/***************** Tags *****************/

#define TID_LENGHT 12

class TagMemory
{
public:
    enum TagMemoryBank{
        NONE = 0,
        RESERVE = 0b0001,
        EPC = 0b0010,
        TID = 0b0100,
        USER = 0b1000
    };
    TagMemory(TagMemoryBank typ = NONE) : memoryBankType(typ){}
    virtual ~TagMemory(){}
    virtual QString toString() const {return "nulltag";}
    const TagMemoryBank memoryBankType;
};

class TagID : public TagMemory
{
public:
    TagID();
    TagID(const TagID & val);
    const TagID &operator =(const TagID & val);
    virtual ~TagID(){}
    QString toString() const;
private:
    uint8_t id[TID_LENGHT];
};

class EPCMem : public TagMemory
{
public:
    EPCMem();
    EPCMem(const EPCMem & val);
    const EPCMem &operator =(const EPCMem & val);
    virtual ~EPCMem(){}
    QString toString() const;
private:
    // todo
};

class User : public TagMemory
{
public:
    User();
    User(const User & val);
    const User &operator =(const User & val);
    virtual ~User(){}
    QString toString() const;
private:
    // todo
};

class Reserve : public TagMemory
{
public:
    Reserve();
    Reserve(const Reserve & val);
    const Reserve &operator =(const Reserve & val);
    virtual ~Reserve(){}
    QString toString() const;
private:
    // todo
};

/***************** Tag ******************/

class Tag : public Serialaizeable
{
public:
    Tag(const TagID&, const EPCMem&, const User&, const Reserve&);
    const TagID & getTagID() const;
    const EPCMem & getEpc() const;
    const User & getUser() const;
    const Reserve & getReserve() const;
    void setEpc(const EPCMem &value);
    void setUser(const User &value);
    void setReserve(const Reserve &value);

    // Serialaizeable interface
    QString toString() const;
    void toJson(QJsonObject &) const;
    void fromJson(const QJsonObject &, QJsonParseError *);

private:
    const TagID tid;
    EPCMem epc;
    User user;
    Reserve reserve;

};


/************* Serialaizeable ************/

class Serialaizeable
{
public:
    Serialaizeable(){}
    virtual ~Serialaizeable(){}
    /*** interface: ***/
    virtual QString toString() const {return QStringLiteral("");}
    virtual void toJson(QJsonObject&) const {}
    virtual void fromJson(const QJsonObject&, QJsonParseError*) {}
};

/***************** CMDs *****************/
//enum CommandsId {
//};


/***************** Event ******************/

class Event : public Serialaizeable
{
public:
    enum EventType{
        ERR = 0,
        TAG = 1};
    Event(EventType event) : event(event) {}
    virtual ~Event(){}

    const EventType event;
    const QDateTime time;
    static qint64 getMSecsSinceEpoch() const;

    // Serialaizeable interface
    virtual QString toString() const;
    virtual void toJson(QJsonObject &) const;
    virtual void fromJson(const QJsonObject &, QJsonParseError *);
};

class TagEvent : public Event
{
public:
    enum TagEventType{
        TAG_LEFT_ZONE = 1,
        TAG_ENTERED_ZONE = 2};

    TagEvent(const QSharedPointer<Tag> &tag, TagEventType evtype) :
        Event(TAG), tagevent(evtype), tag(tag) {}
    virtual ~TagEvent(){}

    // Serialaizeable interface
    virtual QString toString() const;
    virtual void toJson(QJsonObject &) const;
    virtual void fromJson(const QJsonObject &, QJsonParseError *);

    Tag & getTag() const {return *tag;}
    const TagEventType tagevent;
private:
    QSharedPointer<Tag> tag;
};


class ErrEvent : public Event
{

};








#endif // TYPES_H
