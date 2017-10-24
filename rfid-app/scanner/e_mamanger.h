#ifndef READERMANEGNER_H
#define READERMANEGNER_H

#include "d_commands.h"

//#include <QSharedPointer>
#include <QScopedPointer>
#include <QDateTime>
#include <QVariantMap>
#include <QMutex>

//#include <QTimer>


// ********* todo: ********
// make TagStatus child of QObject
// decl TagStatus properties
// make Serializable child of QObject
// decl Serializable child properties
// impl access by mutex
// impl visiable list prop
// ************************
// impl mananger timings.
// ****** BrainFuck *******
// rm virtual method of mananger child; 
// Impl it dynamic by seting property by name;
// Impl getAdditionalPropertys list.
// ************************


/**************** Interface ***************/

/******** TagStatus ********/
class TagStatus: public QObject{
    Q_OBJECT
    
    Q_PROPERTY(QDateTime firstReadTime READ firstReadTime WRITE setFirstReadTime NOTIFY firstReadTimeChanged)
    Q_PROPERTY(QDateTime lastReadTime READ lastReadTime WRITE setLastReadTime NOTIFY lastReadTimeChanged)
    Q_PROPERTY(quint32 readCount READ readCount WRITE setReadCount NOTIFY readCountChanged)
    Q_PROPERTY(qint32 rssi READ rssi WRITE setRssi NOTIFY rssiChanged)
    
public:
    TagStatus(QObject*parent=nullptr):QObject(parent){}
    virtual  ~TagStatus(){}
    
public slots:
    QDateTime firstReadTime() const;
    QDateTime lastReadTime() const;
    quint32 readCount() const;
    qint32 rssi() const;
    
    void setFirstReadTime(QDateTime firstReadTime);
    void setLastReadTime(QDateTime lastReadTime);
    void setReadCount(quint32 readCount);
    void setRssi(qint32 rssi);
    
signals:
    void firstReadTimeChanged(QDateTime firstReadTime);
    void lastReadTimeChanged(QDateTime lastReadTime);
    void readCountChanged(quint32 readCount);
    void rssiChanged(qint32 rssi);

private:
    QDateTime m_firstReadTime;
    QDateTime m_lastReadTime;
    quint32 m_readCount;
    qint32 m_rssi;
};

/******* FieldTimings ******/
class FieldTimings {
    
};

/*** ReaderManengerField ***/
class ReaderManengerField : public QObject{
    Q_OBJECT
public:
//    typedef QMap<QString,QVariant> TagsStatusMap;
    
    ReaderManengerField(QObject *parent = nullptr) : QObject(parent){}
    
public slots:
    void updateField(QStringList readedTags);
    
signals:
    void tagEvent(QSharedPointer<TagEvent> event);
    void fieldEvent(QVariantMap field);
    
private:
    QVariantMap data;
    QMutex acces;
};

/****** ReaderManenger *****/
class ReaderManenger : public Eventful
{
    Q_OBJECT
public:
    //typedef QMap<TagID,TagStatus> ReaderManengerField;

    ReaderManenger(Reader * reader, QObject*parent=nullptr);
    virtual ~ReaderManenger();
    
    // readField->addProcedure->manengeProcedure
    
public slots:
    virtual void start()=0;
    virtual void stop()=0;
    
    const ReaderManengerField &getField() const
    {
        return this->field;
    }
    
signals:
    void fieldChanged(const ReaderManengerField& field);
    
protected:
    QScopedPointer<Reader> reader;
    ReaderManengerField field;
};


/************* Implementation *************/

/*** ReaderManengerSimple **/
class ReaderManengerBasicV1 : public ReaderManenger
{
    Q_OBJECT
public:
    ReaderManengerBasicV1(Reader * reader, QObject *parent=nullptr);
    virtual ~ReaderManengerBasicV1();
    
public slots:
    virtual void run();
    virtual void stop();
    
protected slots:
    //    virtual void onTimer();
    
private:
    //    QTimer timer;
    
};



#endif // READERMANEGNER_H
