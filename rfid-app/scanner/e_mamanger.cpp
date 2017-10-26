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

/*** ScannerManengerField ***/
void ScannerManengerTagField::update(QStringList readedTags)
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
                emit sysEvent(QSharedPointer<Event>(
                                  new ScannerEvent(ScannerEvent::INFO,
                                                   ScannerEvent::MANENGER,
                                                   QStringLiteral("Tag ") +
                                                   fieldIt.peekNext()->tag() +
                                                   " lost(timeout). Last seen: " +
                                                   fieldIt.peekNext()->
                                                   lastReadTime().toString())));
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
                    this, &ScannerManengerTagField::tagStatusHandler);
            m_field.append(newTagStatus);
        }
        emit fieldChanged(m_field);
    }
}
void ScannerManengerTagField::tagStatusHandler(const TagStatus * obj)
{
#define SEC_IN_MIN 60
#define SEC_IN_HOUR (60*SEC_IN_MIN)
#define SEC_IN_DAY (24*SEC_IN_HOUR)

    if(obj->status() == TagStatus::LEAVE) {
        emit sysEvent(QSharedPointer<Event>(
                          new TagLeaveEvent(obj->tag(), obj->lastReadTime())));
                          
        auto sec = obj->firstReadTime().secsTo(obj->lastReadTime());
        uint32_t dd = sec / SEC_IN_DAY;
        sec -= dd * SEC_IN_DAY;
        uint32_t hh = sec / SEC_IN_HOUR;
        sec -= hh * SEC_IN_HOUR;
        uint32_t mm = sec / SEC_IN_MIN;
        sec -= mm * SEC_IN_MIN;
        
        QString elps = QString::number(dd) + "d " + QString::number(hh) + "." +
                       QString::number(mm) + "." + QString::number(sec);
                       
        QString str = QStringLiteral("Tag ") + obj->tag() +
                      " leave. Enter time: " + obj->firstReadTime().toString() +
                      ", last time in zone: " + elps +
                      ", total read count: " + QString::number(obj->readCount()) +
                      " times, percent: " + QString::number(obj->readPercent()) + "%";
        emit sysEvent(QSharedPointer<Event>(
                          new ScannerEvent(ScannerEvent::INFO,
                                           ScannerEvent::MANENGER,
                                           str)));
    } else if(obj->status() == TagStatus::ENTER)
        emit sysEvent(QSharedPointer<Event>(
                          new TagEnterEvent(obj->tag(), obj->firstReadTime())));
}
void ScannerManengerTagField::clear()
{
    m_field.clear();
    emit fieldChanged(m_field);
}
TagFieldTimings & ScannerManengerTagField::timings()
{
    return m_timings;
}
ScannerManengerTagField::TagFieldList ScannerManengerTagField::field() const
{
    return m_field;
}

/*********** ScannerManenger ***********/
ScannerManenger::ScannerManenger(Scanner * scanner, QObject * parent) :
    Eventful(parent), scanner(scanner), tagsfield(this)
{
    connect(&tagsfield, &ScannerManengerTagField::fieldChanged,
            this, &ScannerManenger::fieldChanged);
    tagsfield.connectAsEventDrain(this);
    
    
    this->scanner->setParent(this);
    scanner->connectAsEventDrain(this);
    
    connect(this->scanner.data(), &Scanner::statusChanged,
            this, &ScannerManenger::scannerStatusHandler);
    connect(this, &ScannerManenger::attach,
            this->scanner.data(), &Scanner::attach);
    connect(this, &ScannerManenger::detach,
            this->scanner.data(), &Scanner::detach);
    connect(this, &ScannerManenger::execute,
            this->scanner.data(), &Scanner::execute);
    connect(this->scanner.data(), &Scanner::executed,
            this, &ScannerManenger::executed);
}
ScannerManenger::~ScannerManenger()
{
    stop();
}
void ScannerManenger::stop()
{
    tagsfield.clear();
}
ScannerManengerTagField::TagFieldList ScannerManenger::field() const
{
    return this->tagsfield.field();
}

/******** ScannerManengerSimple ********/
ScannerManengerBasicV1::ScannerManengerBasicV1(Scanner * scanner,
                                             QObject * parent) :
    ScannerManenger(scanner, parent) //, timer(this), scanner(scanner)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}
ScannerManengerBasicV1::~ScannerManengerBasicV1()
{

}
void ScannerManengerBasicV1::start()
{
    //    timer.start(10);
}
void ScannerManengerBasicV1::stop()
{
    //    timer.stop();
}

void ScannerManengerBasicV1::executed(QSharedPointer<ScannerReply>)
{

}
void ScannerManengerBasicV1::onTimer()
{

}



