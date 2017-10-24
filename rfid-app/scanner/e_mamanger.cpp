#include "e_mamanger.h"
#include <algorithm>


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
/*qint32 TagStatus::rssi() const{
    return m_rssi;
}*/
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
}
void TagStatus::setUnreadCount(quint32 unreadCount){
    if (m_unreadCount == unreadCount)
        return;
    
    m_unreadCount = unreadCount;
    emit unreadCountChanged(m_unreadCount);
}
/*void TagStatus::setRssi(qint32 rssi){
    if (m_rssi == rssi)
        return;
        
    m_rssi = rssi;
    emit rssiChanged(m_rssi);
}*/
void TagStatus::wasRead(){
    
}
void TagStatus::wasUnread(){
    
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



