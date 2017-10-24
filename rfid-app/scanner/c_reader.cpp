#include "c_reader.h"

Reader::Reader(ReaderProtocol *protocol, PhyTransport *transport, QObject *parent):
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

/*
bool Reader::getFullMem(Tag &tag)
{
    return (getEPCMem(tag) && getUserMem(tag) && getReserveMem(tag));
}
*/
