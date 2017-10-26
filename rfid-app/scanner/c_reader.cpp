#include "c_reader.h"

Reader::Reader(ReaderProtocol * protocol, PhyTransport * transport,
               QObject * parent):
    Eventful(parent), protocol(protocol), transport(transport)
{
    transport->moveToThread(&thread);
    thread.setObjectName("Reader Phy");
    thread.start();
}

Reader::~Reader()
{
    thread.quit();
    thread.wait();
}

Reader::ReaderStateEnum Reader::status() const
{
    return m_status;
}

void Reader::setStatus(Reader::ReaderStateEnum status)
{
    if(m_status == status)
        return;

    m_status = status;
    emit statusChanged(m_status);
}

/*
bool Reader::getFullMem(Tag &tag)
{
    return (getEPCMem(tag) && getUserMem(tag) && getReserveMem(tag));
}
*/
