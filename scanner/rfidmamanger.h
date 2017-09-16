#ifndef RFIDMAMANGER_H
#define RFIDMAMANGER_H

#include "rfidreader.h"
#include "types.h"

#include <QSharedPointer>
#include <QList>
#include <QDateTime>
#include <QThread>


class TagStatus
{

public:
    QDateTime getLastVisiagleTime() const;
    void setLastVisiagleTime(const QDateTime &value);
    uint getReadCount() const;
    void setReadCount(const uint &value);
    int getRssi() const;
    void setRssi(int value);

private:
    QDateTime lastVisiagleTime;
    uint readCount;
    int rssi;
};

class RFIDMamanger : public QObject
{
    Q_OBJECT
public:
    typedef QMap<TagID,TagStatus> TagField;

    RFIDMamanger(QSharedPointer<RFIDReader> reader);
    ~RFIDMamanger();

public slots:
    virtual void run(){}
    virtual void stop(){}

signals:
    void fieldChanged(const TagField* tags);
    void fieldEvent(QSharedPointer<TagEvent> event);
    void otherEvent(QSharedPointer<InfoEvent> event);

private:
    TagField field;
    QSharedPointer<RFIDReader> reader;
    QThread * thread;
};


class SimpleRFIDMamanger : public RFIDMamanger
{
    Q_OBJECT
public:
    void run() final;
};



#endif // RFIDMAMANGER_H
