#ifndef READERMANEGNER_H
#define READERMANEGNER_H

#include "rfidreader.h"
#include "types.h"

#include <QSharedPointer>
#include <QMap>
#include <QMutex>
#include <QDateTime>
#include <QThread>
#include <QTimer>


/**************** Interface ***************/

/******** TagStatus ********/
class TagStatus: public QObject{
    Q_OBJECT
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



// ********* todo: ********
// make TagStatus child of QObject
// decl TagStatus properties
// make Serializable child of QObject
// decl Serializable child properties
// impl access by mutex
// impl visiable list prop
// ****** BrainFuck *******
// impl mananger timings.
// rm virtual method of NetClient; 
// Impl it dynamic by seting property by name;
// Impl getAdditionalPropertys list.
// ************************
/* ReaderManengerTagsStatus */
class ReaderManengerTagsStatus : public QObject{
    Q_OBJECT
public:
    ReaderManengerTagsStatus(QObject *parent = nullptr) : 
        QObject(parent), data(){}
    
    
private:
    QMap<TagID,TagStatus> data;
    QMutex acces;
};

/****** ReaderManenger *****/
class ReaderManenger : public QObject
{// readField->addProcedure->manengeProcedure
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


/************* Implementation *************/

/*** ReaderManengerSimple **/
class ReaderManengerSimple : public ReaderManenger
{
    Q_OBJECT
public:
    ReaderManengerSimple(QSharedPointer<Reader> reader);
    virtual ~ReaderManengerSimple();

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
