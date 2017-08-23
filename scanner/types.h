#ifndef TYPES_H
#define TYPES_H

#include <inttypes.h>
#include <QJsonObject>
#include <QString>
#include <QDateTime>

#define TID_LENGHT 12

class TagID
{
public:
    TagID(const TagID & id);
    QString toString() const;

private:
    uint8_t id[TID_LENGHT];
};

class Tag
{
public:
    void writeToJson(QJsonObject &json) const;

private:
    TagID tid;
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
