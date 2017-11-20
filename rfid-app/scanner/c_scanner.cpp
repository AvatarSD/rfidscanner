#include "c_scanner.h"


/********************** Level 3(C) ***********************/

/*************** Interface ****************/

/********** Scanner *********/
Scanner::Scanner(QObject * parent): Eventful(parent)
{
    setStatus(DETHACHED);
}
Scanner::~Scanner()
{}
Scanner::ScannerStateEnum Scanner::status() const{
    return m_status;
}
void Scanner::setStatus(Scanner::ScannerStateEnum status){
    if(m_status == status)
        return;
    
    m_status = status;
    emit statusChanged(m_status);
}

/*
bool Scanner::getFullMem(Tag &tag)
{
    return (getEPCMem(tag) && getUserMem(tag) && getReserveMem(tag));
}
*/


/************ Implementation **************/

/***** ScannerEmulator *****/
ScannerEmulator::ScannerEmulator(QObject *parent) : 
    Scanner(parent), inspectTimer(parent),
    readPercentMin(10), readPercentMax(70), 
    tagsCountMin(3), tagsCountMax(15),
    tagNewMinTime(5), tagNewMaxTime(120),
    tagMinTime(10), tagMaxTime(180),
    inspectTimeMsec(500)
{
    srand (time(NULL));
    connect(&inspectTimer, &QTimer::timeout,
            this, &ScannerEmulator::inspect);
    timeToNextNew = QDateTime::currentSecsSinceEpoch();
    setStatus(DETHACHED);
}
QString ScannerEmulator::isAddrValid(QString addr){
    return QStringLiteral(""); //Valid: ") + addr;
}
QStringList ScannerEmulator::validAddrList(){
    auto lst = getRandAddrs();
    emit validAddrListChanged(lst);
    return lst;
}
void ScannerEmulator::attach(QString){
    detach();
    QThread::msleep(100);
    setStatus(ATTACHED);
    QThread::msleep(500);
    inspectTimer.start(inspectTimeMsec);
    setStatus(READY);
}
void ScannerEmulator::detach(){
    inspectTimer.stop();
    tagList.clear();
    setStatus(DETHACHED);
}
void ScannerEmulator::execute(QSharedPointer<ScannerRequest> request)
{
    if(status()==DETHACHED)
        return;
    
    switch (request->type()){
    case ScannerCommand::RUN: 
        inspectTimer.start(inspectTimeMsec);
        setStatus(READY);
        break;
    case ScannerCommand::SLEEP: 
        inspectTimer.stop();
        setStatus(ATTACHED);
        break;
    case ScannerCommand::STATUS: 
        QTimer::singleShot(0, this, &ScannerEmulator::statusRq);
        break;
    }
    
    if(status()!=ATTACHED)
        return;
    
    switch (request->type()){
    case ScannerCommand::INVENTORY: 
        QTimer::singleShot(0, this, &ScannerEmulator::inventoryRq);
        break;
    }
}
bool ScannerEmulator::isRead(){
    return getRandPercent() < ((rand()%(readPercentMax-readPercentMin))+readPercentMin);
}
void ScannerEmulator::newTag(){
    QString tag;
    for(int i = 0; i < 12; i++)
        tag += QString::number(rand()%0xff, 16) + (i < 11 ? ":" : "");
    tag.toUpper();
    timeToNextNew = QDateTime::currentSecsSinceEpoch() + 
            ((rand()%(tagNewMaxTime-tagNewMinTime))+tagNewMinTime);
    tagList[tag] = QDateTime::currentSecsSinceEpoch() + 
            ((rand()%(tagMaxTime-tagMinTime))+tagMinTime);
}
QStringList ScannerEmulator::getRandAddrs(){
    QStringList lst;
    lst << "Foo" << "Bar" << "Other" << "Test" << "Rand";
    return lst;
}
uint ScannerEmulator::getRandPercent(){
    return rand()%100;;
}
void ScannerEmulator::inspect(){
    if((tagList.count() < tagsCountMax) && 
            (QDateTime::currentSecsSinceEpoch() > timeToNextNew))
        newTag();
    if(tagList.count() > tagsCountMin)
        for (auto var = tagList.begin(); var != tagList.end(); var ++)
            if(QDateTime::currentSecsSinceEpoch() > var.value())
                tagList.erase(var);
    
}
void ScannerEmulator::inventoryRq(){
    QSharedPointer<InventoryRp> reply(new InventoryRp);
    for (auto var = tagList.begin(); var != tagList.end(); var ++)
        if(isRead())
            reply->tags.append(var.key());
    QThread::msleep(3);
    emit executed(reply);
}
void ScannerEmulator::statusRq(){
    QSharedPointer<StatusRp> reply(new StatusRp);
    QThread::msleep(10);
    emit executed(reply);
}
