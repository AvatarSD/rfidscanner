#ifndef READERMANEGNER_H
#define READERMANEGNER_H

#include "d_commands.h"

#include <QSharedPointer>
#include <QScopedPointer>
#include <QDateTime>
#include <QList>
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

/****** Default value ******/
#define DEFAULT_TAG_LEFT_MSEC 30000
#define DEFAULT_TAG_LEFT_PCNT 20
#define DEFAULT_TAG_LEFT_RULE TagFieldLeftRule::TIME
#define DEFAULT_TAG_DEL_SEC   300

/**** TagFieldLeftRules ****/
class TagFieldLeftRule : QObject{
    Q_OBJECT
public:
    enum TagFieldLeftRuleEnum {
        TIME,
        PERCENT,
        TIME_AND_PERCENT,
        TIME_OR_PERCENT
    };
    Q_ENUM(TagFieldLeftRuleEnum)
};

/******* FieldTimings ******/
class TagFieldTimings {
public:
    TagFieldTimings();
    uint maxUnreadToLeftMsec;
    uint maxUnreadToLeftPcnt;
    uint maxUnreadToDeleteSec;
    TagFieldLeftRule::TagFieldLeftRuleEnum leftRule;
};

/******** TagStatus ********/
class TagStatus: public QObject{
    Q_OBJECT
    Q_PROPERTY(QString       tag           READ tag)
    Q_PROPERTY(TagStatusEnum status        READ status         NOTIFY statusChanged)
    Q_PROPERTY(QDateTime     firstReadTime READ firstReadTime  NOTIFY firstReadTimeChanged)
    Q_PROPERTY(QDateTime     lastReadTime  READ lastReadTime   NOTIFY lastReadTimeChanged)
    Q_PROPERTY(quint32       readCount     READ readCount      NOTIFY readCountChanged)
    Q_PROPERTY(quint32       unreadCount   READ unreadCount    NOTIFY unreadCountChanged)
    Q_PROPERTY(float         readPercent   READ readPercent    NOTIFY readPercentChanged)
public:
    enum TagStatusEnum{
        ONLINE,
        OFFLINE,
        LEFT
    };
    Q_ENUM(TagStatusEnum)
    TagStatus(QString tag, QObject*parent=nullptr):
        QObject(parent), m_tag(tag), m_status(ONLINE),
        m_firstReadTime(QDateTime::currentDateTime()),
        m_lastReadTime(QDateTime::currentDateTime()),
        m_readCount(1), m_unreadCount(0)  {}
    virtual  ~TagStatus(){}
    float readPercent() const;
    
public slots:
    QString tag() const;
    TagStatusEnum status() const;
    QDateTime firstReadTime() const;
    QDateTime lastReadTime() const;
    quint32 readCount() const;
    quint32 unreadCount() const;
    void wasRead();
    void wasUnread(const TagFieldTimings &timings);
private:
    void setStatus(TagStatusEnum status);
    void setFirstReadTime(QDateTime firstReadTime);
    void setLastReadTime(QDateTime lastReadTime);
    void setReadCount(quint32 readCount);
    void setUnreadCount(quint32 unreadCount);
signals:
    void firstReadTimeChanged(QDateTime firstReadTime);
    void lastReadTimeChanged(QDateTime lastReadTime);
    void readCountChanged(quint32 readCount);
    void unreadCountChanged(quint32 unreadCount);   
    void statusChanged(TagStatusEnum status);
    void readPercentChanged(float readPercent);
    
private:
    const QString m_tag;
    TagStatusEnum m_status;
    QDateTime m_firstReadTime;
    QDateTime m_lastReadTime;
    quint32 m_readCount;
    quint32 m_unreadCount;
};

/*** ReaderManengerField ***/
class ReaderManengerTagField : public QObject{
    Q_OBJECT
public:
    typedef QList<QSharedPointer<TagStatus>> TagFieldList; //ok!!
    
    ReaderManengerTagField(QObject *parent = nullptr) : 
        QObject(parent){}
    virtual ~ReaderManengerTagField(){}
    
    TagFieldList field() const;
    TagFieldTimings * timings();

public slots:
    void updateField(QStringList readedTags);
    
signals:
    void tagEvent(QSharedPointer<TagEvent> event);
    void fieldChanged(TagFieldList field);
    
private:
    TagFieldList m_field;
    TagFieldTimings m_timings;
    mutable QMutex access;
};

/****** ReaderManenger *****/
class ReaderManenger : public Eventful
{
    Q_OBJECT
public:
    ReaderManenger(Reader * reader, QObject*parent=nullptr);
    virtual ~ReaderManenger();
    
    // readField->addProcedure->manengeProcedure
    
public slots:
    virtual void start()=0;
    virtual void stop()=0;
    
    const ReaderManengerTagField &getField() const{
        return this->field;
    }
    
signals:
    void fieldChanged(const ReaderManengerTagField& field);
    
protected:
    QScopedPointer<Reader> reader;
    ReaderManengerTagField field;
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
