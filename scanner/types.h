#ifndef TYPES_H
#define TYPES_H

#include <inttypes.h>
#include <QJsonObject>
#include <QString>
#include <QDateTime>

#define TID_LENGHT 12


class TagMemory
{
public:
    TagMemory(){}
    virtual ~TagMemory(){}
    enum TagMemoryBank{
        RESERVE = 0b00,
        EPC = 0b01,
        TID = 0b10,
        USEER = 0b11
    };
    virtual TagMemoryBank getMemoryBankNumber() const{return (TagMemoryBank)-1;}
    virtual QString toString() const {return "";}
};


class TagID : public TagMemory
{
public:
    TagID(const TagID & id);
    TagMemory::TagMemoryBank getMemoryBankNumber() const;
    QString toString() const;
private:
    uint8_t id[TID_LENGHT];
};

class EPCMem : public TagMemory
{
public:
    TagMemory::TagMemoryBank getMemoryBankNumber() const;
    QString toString() const;
};

class User : public TagMemory
{
public:
    TagMemory::TagMemoryBank getMemoryBankNumber() const;
    QString toString() const;
};

class Reserve : public TagMemory
{
public:
    TagMemory::TagMemoryBank getMemoryBankNumber() const;
    QString toString() const;
};


class Tag
{
public:
    Tag(const TagID&, const EPCMem&, const User&, const Reserve&);
    void writeToJson(QJsonObject &json) const;
    const TagID & getTagID() const;
    const EPCMem & getEpc() const;
    const User & getUser() const;
    const Reserve & getReserve() const;
    void setEpc(const EPCMem &value);
    void setUser(const User &value);
    void setReserve(const Reserve &value);

private:
    const TagID tid;
    EPCMem epc;
    User user;
    Reserve reserve;
};


class LogEvent
{
public:
    enum EventType{
        LEFT_ZONE = 1,
        ENTERED_ZONE = 2
    };

    LogEvent(const Tag &id, EventType evtype);
    ~LogEvent();

//    QString readToString();
//    void readFromJson(QJsonObject &json) const;
    void writeToJson(QJsonObject &json) const;
    Tag getTag() const;
    EventType getEvent() const;
    qint64 getMSecsSinceEpoch() const;

private:
    const Tag tag;
    const EventType event;
    const QDateTime time;
};

/*class LogEventMessage
{

};*/












#endif // TYPES_H
