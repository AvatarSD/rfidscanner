#ifndef RFIDMAMANGER_H
#define RFIDMAMANGER_H

#include "rfidreader.h"
#include "types.h"

#include <QSharedPointer>
#include <QList>
#include <QDateTime>
#include <QThread>
#include <QTimer>

/************* TagStatus *************/
class TagStatus
{

public:
    QDateTime getLastVisiableTime() const;
    void setLastVisiableTime(const QDateTime &value);
    uint getReadCount() const;
    void setReadCount(const uint &value);
    int getRssi() const;
    void setRssi(int value);

private:
    QDateTime lastVisiableTime;
    uint readCount;
    int rssi;
};


/*********** RFIDManamger ***********/
class RFIDMamanger : public QObject
{
    Q_OBJECT
public:
    typedef QMap<TagID,TagStatus> TagField;

    RFIDMamanger();
    virtual ~RFIDMamanger();

public slots:
    virtual void run(){}
    virtual void stop(){}

signals:
    void fieldChanged(const TagField* tags);
//    void fieldEvent(QSharedPointer<TagEvent> event);
//    void otherEvent(QSharedPointer<InfoEvent> event);

protected:
    QThread thread;
};


/******** SimpleRFIDMamanger ********/
class SimpleRFIDMamanger : public RFIDMamanger
{
    Q_OBJECT
public:
    SimpleRFIDMamanger(QSharedPointer<RFIDReader> reader);
    virtual ~SimpleRFIDMamanger();

public slots:
    virtual void run();
    virtual void stop();

protected slots:
    virtual void onTimer();

private:
    QTimer timer;
    TagField field;
    QSharedPointer<RFIDReader> reader;
};



#endif // RFIDMAMANGER_H
