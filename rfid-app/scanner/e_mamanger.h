#ifndef READERMANEGNER_H
#define READERMANEGNER_H

#include <QSharedPointer>
#include <QScopedPointer>
#include <QDateTime>
#include <QList>
#include <QMutex>
//#include <QTimer>

#include "d_commands.h"

#define MAX_PERCENT 100.0

/**************** Interface ***************/

/****** Default value ******/
#define DEFAULT_TAG_LEFT_MSEC 5000
#define DEFAULT_TAG_LEFT_PCNT 20
#define DEFAULT_TAG_LEFT_RULE TagFieldLeftRule::TIME
#define DEFAULT_TAG_DEL_SEC   300

/**** TagFieldLeftRules ****/
class TagFieldLeftRule : QObject
{
    Q_OBJECT
public:
    virtual ~TagFieldLeftRule() {}
    enum TagFieldLeftRuleEnum {
        TIME,
        PERCENT,
        TIME_AND_PERCENT,
        TIME_OR_PERCENT
    };
    Q_ENUM(TagFieldLeftRuleEnum)
};

/******* FieldTimings ******/
class TagFieldTimings
{
public:
    TagFieldTimings();
    uint maxUnreadToLeftMsec;
    uint maxUnreadToLeftPcnt;
    uint maxUnreadToDeleteSec;
    TagFieldLeftRule::TagFieldLeftRuleEnum leftRule;
};

/******** TagStatus ********/
class TagStatus: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString       tag           READ tag)
    Q_PROPERTY(TagStatusEnum status        READ status         NOTIFY statusChanged)
    Q_PROPERTY(QDateTime     firstReadTime READ firstReadTime  NOTIFY
               firstReadTimeChanged)
    Q_PROPERTY(QDateTime     lastReadTime  READ lastReadTime   NOTIFY
               lastReadTimeChanged)
    Q_PROPERTY(quint32       readCount     READ readCount      NOTIFY
               readCountChanged)
    Q_PROPERTY(quint32       unreadCount   READ unreadCount    NOTIFY
               unreadCountChanged)
    Q_PROPERTY(float         readPercent   READ readPercent    NOTIFY
               readPercentChanged)
public:
    enum TagStatusEnum {
        ENTER,
        UNREAD,
        LEAVE
    };
    Q_ENUM(TagStatusEnum)
    TagStatus(QString tag, QObject * parent = nullptr):
        QObject(parent), m_tag(tag), m_status(ENTER),
        m_firstReadTime(QDateTime::currentDateTime()),
        m_lastReadTime(QDateTime::currentDateTime()),
        m_readCount(1), m_unreadCount(0)  {}
    virtual  ~TagStatus() {}

public slots:
    QString tag() const;
    TagStatusEnum status() const;
    QDateTime firstReadTime() const;
    QDateTime lastReadTime() const;
    quint32 readCount() const;
    quint32 unreadCount() const;
    float readPercent() const;
    void wasRead();
    void wasUnread(const TagFieldTimings & timings);
private:
    void setStatus(TagStatusEnum status);
    void setFirstReadTime(QDateTime firstReadTime);
    void setLastReadTime(QDateTime lastReadTime);
    void setReadCount(quint32 readCount);
    void setUnreadCount(quint32 unreadCount);
signals:
    void statusEvent(const TagStatus * obj);
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
class ReaderManengerTagField : public Eventful
{
    Q_OBJECT
public:
    typedef QList<QSharedPointer<TagStatus>> TagFieldList; //ok!!
    ReaderManengerTagField(QObject * parent = nullptr) :
        Eventful(parent) {}
    virtual ~ReaderManengerTagField() {}
public slots:
    void update(QStringList readedTags);
    void clear();
    TagFieldTimings & timings();
    TagFieldList field() const;
signals:
    void fieldChanged(TagFieldList field);
private slots:
    void tagStatusHandler(const TagStatus * obj);
private:
    TagFieldList m_field;
    TagFieldTimings m_timings;
};

/****** ReaderManenger *****/
class ReaderManenger : public Eventful
{
    Q_OBJECT
public:
    ReaderManenger(Reader * reader, QObject * parent = nullptr);
    virtual ~ReaderManenger();
public slots:
    virtual void start() = 0;
    virtual void stop();
    ReaderManengerTagField::TagFieldList field() const;
signals:
    void fieldChanged(ReaderManengerTagField::TagFieldList field);
    /********************/
protected slots:
    virtual void executed(QSharedPointer<ScannerReply>) = 0;
signals:
    void execute(QSharedPointer<ScannerRequest>);
protected:
    QScopedPointer<Reader> reader;
    ReaderManengerTagField tagsfield;
};


/************* Implementation *************/

/*** ReaderManengerSimple **/
class ReaderManengerBasicV1 : public ReaderManenger
{
    Q_OBJECT
public:
    ReaderManengerBasicV1(Reader * reader, QObject * parent = nullptr);
    virtual ~ReaderManengerBasicV1();

public slots:
    virtual void start();
    virtual void stop();

protected slots:
    virtual void executed(QSharedPointer<ScannerReply>);
    void onTimer();
private:
    QTimer timer;
};



#endif // READERMANEGNER_H
