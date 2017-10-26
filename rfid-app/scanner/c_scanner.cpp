#include "c_scanner.h"

Scanner::Scanner(ScannerProtocol * protocol, PhyTransport * transport,
               QObject * parent):
    Eventful(parent), protocol(protocol), transport(transport)
{
    transport->moveToThread(&thread);
    thread.setObjectName("Scanner Phy");
    thread.start();
}

Scanner::~Scanner()
{
    thread.quit();
    thread.wait();
}

Scanner::ScannerStateEnum Scanner::status() const
{
    return m_status;
}

void Scanner::setStatus(Scanner::ScannerStateEnum status)
{
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
