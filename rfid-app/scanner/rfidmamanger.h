#ifndef READERMANEGNER_H
#define READERMANEGNER_H

#include "rfidreader.h"
#include "types.h"

#include <QSharedPointer>
#include <QMap>
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


/*********** ReaderManenger ***********/
class ReaderManenger : public QObject
{
    Q_OBJECT
public:
    typedef QMap<TagID,TagStatus> TagField;

    ReaderManenger();
    virtual ~ReaderManenger();

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


/******** SimpleReaderManenger ********/
class SimpleReaderManenger : public ReaderManenger
{
    Q_OBJECT
public:
    SimpleReaderManenger(QSharedPointer<Reader> reader);
    virtual ~SimpleReaderManenger();

public slots:
    virtual void run();
    virtual void stop();

protected slots:
    virtual void onTimer();

private:
    QTimer timer;
    TagField field;
    QSharedPointer<Reader> reader;
};



#endif // READERMANEGNER_H
