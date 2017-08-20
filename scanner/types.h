#ifndef TYPES_H
#define TYPES_H

#include <inttypes.h>
#include <QJsonObject>
#include <QString>



class TagID
{
public:
    QString toString();

private:
    uint8_t id[12]
};

class Tag
{
public:
    void writeToJson(QJsonObject &json);

private:
    TagID tid;
};

enum EventType
{
    LEFT_ZONE = 1,
    ENTERED_ZONE = 2
};

class LogEvent
{
public:
    LogEvent(const TagID &id, EventType evtype, uint64_t unixtime);
    ~LogEvent();

//    QString readToString();
//    void readFromJson(QJsonObject &json) const;
    void writeToJson(QJsonObject &json);
    Tag getTag() const;
    EventType getEvent() const;
    uint64_t getTimestamp() const;

private:
    Tag tag;
    EventType event;
    uint64_t timestamp;
};

/*class LogEventMessage
{

};*/












#endif // TYPES_H
