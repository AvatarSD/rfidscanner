#include "e_mamanger.h"
#include <algorithm>

#define MAX_PERCENT 100.0

/************* TagStatus *************/

QString TagStatus::tag() const{
    return m_tag;
}
TagStatus::TagStatusEnum TagStatus::status() const{
    return m_status;
}
QDateTime TagStatus::firstReadTime() const{
    return m_firstReadTime;
}
QDateTime TagStatus::lastReadTime() const{
    return m_lastReadTime;
}
quint32 TagStatus::readCount() const{
    return m_readCount;
}
quint32 TagStatus::unreadCount() const{
    return m_unreadCount;
}
float TagStatus::readPercent() const{
    return (((float)readCount())/((float)(readCount()+unreadCount())))*MAX_PERCENT;
}
void TagStatus::setStatus(TagStatus::TagStatusEnum status){
    if (m_status == status)
        return;
    
    m_status = status;
    emit statusChanged(m_status);
}
void TagStatus::setFirstReadTime(QDateTime firstReadTime){
    if (m_firstReadTime == firstReadTime)
        return;
    
    m_firstReadTime = firstReadTime;
    emit firstReadTimeChanged(m_firstReadTime);
}
void TagStatus::setLastReadTime(QDateTime lastReadTime){
    if (m_lastReadTime == lastReadTime)
        return;
    
    m_lastReadTime = lastReadTime;
    emit lastReadTimeChanged(m_lastReadTime);
}
void TagStatus::setReadCount(quint32 readCount){
    if (m_readCount == readCount)
        return;
    
    m_readCount = readCount;
    emit readCountChanged(m_readCount);
    emit readPercentChanged(readPercent());
}
void TagStatus::setUnreadCount(quint32 unreadCount){
    if (m_unreadCount == unreadCount)
        return;
    
    m_unreadCount = unreadCount;
    emit unreadCountChanged(m_unreadCount);
    emit readPercentChanged(readPercent());
}
void TagStatus::wasRead(const TagFieldTimings &timings){
    setReadCount(readCount()++);
    if(status() == LEFT) 
        setFirstReadTime(QDateTime::currentDateTime());
    setLastReadTime(QDateTime::currentDateTime());
    setStatus(ONLINE);
}
void TagStatus::wasUnread(const TagFieldTimings &timings){
    setUnreadCount(unreadCount()++);
    if(status() == ONLINE)
        setStatus(OFFLINE);
    else if(status() == OFFLINE){
        bool isTimeOut = (lastReadTime().msecsTo(QDateTime::currentDateTime()) 
                          >= timings.maxUnreadToLeftMsec);
        bool isPcntLow = (readPercent() < timings.maxUnreadToLeftPcnt);
        bool isLeft = false;
        switch (timings.leftRule) {
        case TagFieldLeftRule::TIME:             isLeft = isTimeOut; break;
        case TagFieldLeftRule::PERCENT:          isLeft = isPcntLow; break;
        case TagFieldLeftRule::TIME_AND_PERCENT: isLeft = isTimeOut && isPcntLow; break;
        case TagFieldLeftRule::TIME_OR_PERCENT:  isLeft = isTimeOut || isPcntLow; break;
        }
        if(isLeft)
            setStatus(LEFT);
    }
}

/******* FieldTimings ******/
TagFieldTimings::TagFieldTimings() : maxUnreadToLeftMsec(DEFAULT_TAG_LEFT_MSEC),
    maxUnreadToLeftPcnt(DEFAULT_TAG_LEFT_PCNT), maxUnreadToDeleteSec(DEFAULT_TAG_DEL_SEC),
    leftRule(DEFAULT_TAG_LEFT_RULE){}

/*** ReaderManengerField ***/
void ReaderManengerTagField::updateField(QStringList readedTags)
{
    QMutexLocker lock(&access);
    
    /* iterate m_field{
 * 
 *     iterate readedTags
 * 
 * }
 * 
*/
    
    foreach (auto tags, readedTags) {
        
        
    }
    
    
    
}
TagFieldTimings *ReaderManengerTagField::timings(){
    return &m_timings;
}
ReaderManengerTagField::TagFieldList ReaderManengerTagField::field() const{
    QMutexLocker lock(&access);
    return m_field;
}

/*********** ReaderManenger ***********/
ReaderManenger::ReaderManenger(Reader *reader, QObject *parent) :
    Eventful(parent), reader(reader)
{
    
}
ReaderManenger::~ReaderManenger()
{
    
}

/******** ReaderManengerSimple ********/
ReaderManengerBasicV1::ReaderManengerBasicV1(Reader * reader, QObject *parent) :
    ReaderManenger(reader,parent)//, timer(this), reader(reader)
{
    //    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}
ReaderManengerBasicV1::~ReaderManengerBasicV1()
{
    
}
void ReaderManengerBasicV1::run()
{
    //    timer.start(10);
}
void ReaderManengerBasicV1::stop()
{
    //    timer.stop();
}
//void ReaderManengerBasicV1::onTimer()
//{

//}



