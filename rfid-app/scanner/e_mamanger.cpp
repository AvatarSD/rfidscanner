#include "e_mamanger.h"


/************* TagStatus *************/

QDateTime TagStatus::firstReadTime() const{
    return m_firstReadTime;
}
QDateTime TagStatus::lastReadTime() const{
    return m_lastReadTime;
}
quint32 TagStatus::readCount() const{
    return m_readCount;
}
qint32 TagStatus::rssi() const{
    return m_rssi;
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
void TagStatus::setRssi(qint32 rssi){
    if (m_rssi == rssi)
        return;
    
    m_rssi = rssi;
    emit rssiChanged(m_rssi);
}

/*** ReaderManengerField ***/
void ReaderManengerField::updateField(QStringList readedTags)
{
    
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



