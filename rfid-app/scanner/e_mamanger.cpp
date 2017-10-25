#include "e_mamanger.h"
#include <algorithm>


/************* TagStatus *************/
QString TagStatus::tag() const
{
    return m_tag;
}
TagStatus::TagStatusEnum TagStatus::status() const
{
    return m_status;
}
QDateTime TagStatus::firstReadTime() const
{
    return m_firstReadTime;
}
QDateTime TagStatus::lastReadTime() const
{
    return m_lastReadTime;
}
quint32 TagStatus::readCount() const
{
    return m_readCount;
}
quint32 TagStatus::unreadCount() const
{
    return m_unreadCount;
}
float TagStatus::readPercent() const
{
    return (((float)readCount()) / ((float)(readCount() + unreadCount()))) *
           MAX_PERCENT;
}
void TagStatus::setStatus(TagStatus::TagStatusEnum status)
{
    if(m_status == status)
        return;
    bool isEvent = status == LEAVE || (status == ENTER && m_status == LEAVE);
    m_status = status;
    emit statusChanged(m_status);
    if(isEvent)
        emit statusEvent(this);
}
void TagStatus::setFirstReadTime(QDateTime firstReadTime)
{
    if(m_firstReadTime == firstReadTime)
        return;

    m_firstReadTime = firstReadTime;
    emit firstReadTimeChanged(m_firstReadTime);
}
void TagStatus::setLastReadTime(QDateTime lastReadTime)
{
    if(m_lastReadTime == lastReadTime)
        return;

    m_lastReadTime = lastReadTime;
    emit lastReadTimeChanged(m_lastReadTime);
}
void TagStatus::setReadCount(quint32 readCount)
{
    if(m_readCount == readCount)
        return;

    m_readCount = readCount;
    emit readCountChanged(m_readCount);
    emit readPercentChanged(readPercent());
}
void TagStatus::setUnreadCount(quint32 unreadCount)
{
    if(m_unreadCount == unreadCount)
        return;

    m_unreadCount = unreadCount;
    emit unreadCountChanged(m_unreadCount);
    emit readPercentChanged(readPercent());
}
void TagStatus::wasRead()
{
    setReadCount(readCount() + 1);
    if(status() == LEAVE)
        setFirstReadTime(QDateTime::currentDateTime());
    setLastReadTime(QDateTime::currentDateTime());
    setStatus(ENTER);
}
void TagStatus::wasUnread(const TagFieldTimings & timings)
{
    if(status() != LEAVE)
        setUnreadCount(unreadCount() + 1);
    if(status() == ENTER)
        setStatus(UNREAD);
    else if(status() == UNREAD) {
        bool isTimeOut = (lastReadTime().msecsTo(QDateTime::currentDateTime())
                          >= timings.maxUnreadToLeftMsec);
        bool isPcntLow = (readPercent() < timings.maxUnreadToLeftPcnt);
        bool isLeft = false;
        switch(timings.leftRule) {
        case TagFieldLeftRule::TIME:             isLeft = isTimeOut; break;
        case TagFieldLeftRule::PERCENT:          isLeft = isPcntLow; break;
        case TagFieldLeftRule::TIME_AND_PERCENT: isLeft = isTimeOut && isPcntLow; break;
        case TagFieldLeftRule::TIME_OR_PERCENT:  isLeft = isTimeOut || isPcntLow; break;
        }
        if(isLeft)
            setStatus(LEAVE);
    }
}

/******* FieldTimings ******/
TagFieldTimings::TagFieldTimings() : maxUnreadToLeftMsec(DEFAULT_TAG_LEFT_MSEC),
    maxUnreadToLeftPcnt(DEFAULT_TAG_LEFT_PCNT),
    maxUnreadToDeleteSec(DEFAULT_TAG_DEL_SEC),
    leftRule(DEFAULT_TAG_LEFT_RULE) {}

/*** ReaderManengerField ***/
void ReaderManengerTagField::update(QStringList readedTags)
{
    QMutableListIterator<QSharedPointer<TagStatus>> fieldIt(m_field);
    while(fieldIt.hasNext()) {
        int readedTadIndex = readedTags.indexOf(fieldIt.peekNext()->tag());
        if(readedTadIndex >= 0) {
            fieldIt.peekNext()->wasRead();
            readedTags.removeAt(readedTadIndex);
        } else {
            fieldIt.peekNext()->wasUnread(m_timings);
            if(fieldIt.peekNext()->lastReadTime().secsTo(QDateTime::currentDateTime())
                    >= m_timings.maxUnreadToDeleteSec) {
                fieldIt.remove();
                emit fieldChanged(m_field);
            }
        }
        fieldIt.next();
    }
    if(readedTags.size() > 0) {
        foreach(auto tag, readedTags) {
            QSharedPointer<TagStatus> newTagStatus(new TagStatus(tag, this));
            connect(newTagStatus.data(), &TagStatus::statusEvent,
                    this, &ReaderManengerTagField::tagStatusHandler);
            m_field.append(newTagStatus);
        }
        emit fieldChanged(m_field);
    }
}
void ReaderManengerTagField::tagStatusHandler(const TagStatus * obj)
{
    if(obj->status() == TagStatus::LEAVE)
        emit sysEvent(QSharedPointer<Event>(
                          new TagLeaveEvent(obj->tag(), obj->lastReadTime())));
    else if(obj->status() == TagStatus::ENTER)
        emit sysEvent(QSharedPointer<Event>(
                          new TagEnterEvent(obj->tag(), obj->firstReadTime())));
}
void ReaderManengerTagField::clear()
{
    m_field.clear();
    emit fieldChanged(m_field);
}
TagFieldTimings & ReaderManengerTagField::timings()
{
    return m_timings;
}
ReaderManengerTagField::TagFieldList ReaderManengerTagField::field() const
{
    return m_field;
}

/*********** ReaderManenger ***********/
ReaderManenger::ReaderManenger(Reader * reader, QObject * parent) :
    Eventful(parent), reader(reader), tagsfield(this)
{
    connect(&tagsfield, &ReaderManengerTagField::fieldChanged,
            this, &ReaderManenger::fieldChanged);
    tagsfield.connectAsEventDrain(this);


    this->reader->setParent(this);
    reader->connectAsEventDrain(this);

    connect()
}
ReaderManenger::~ReaderManenger()
{
    stop();
}
void ReaderManenger::stop()
{
    tagsfield.clear();
}
ReaderManengerTagField::TagFieldList ReaderManenger::field() const
{
    return this->tagsfield.field();
}

/******** ReaderManengerSimple ********/
ReaderManengerBasicV1::ReaderManengerBasicV1(Reader * reader,
                                             QObject * parent) :
    ReaderManenger(reader, parent) //, timer(this), reader(reader)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}
ReaderManengerBasicV1::~ReaderManengerBasicV1()
{

}
void ReaderManengerBasicV1::start()
{
    //    timer.start(10);
}
void ReaderManengerBasicV1::stop()
{
    //    timer.stop();
}

void ReaderManengerBasicV1::executed(QSharedPointer<ScannerReply>)
{

}
void ReaderManengerBasicV1::onTimer()
{

}



