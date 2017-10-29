#include "c_scanner.h"


/********************** Level 3(C) ***********************/

/*************** Interface ****************/

/********** Scanner *********/
Scanner::Scanner(QObject * parent): Eventful(parent)
{}
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
QString ScannerEmulator::isAddrValid(QString addr)
{
}
QStringList ScannerEmulator::validAddrList()
{
}
void ScannerEmulator::attach(QString addr)
{
}
void ScannerEmulator::detach()
{
}
void ScannerEmulator::execute(QSharedPointer<ScannerRequest>)
{
}