#include "rfidreader.h"

Reader::Reader(QSharedPointer<Protocol> prot, QSharedPointer<PhyTransport> trnsp):
    protocol(prot), transport(trnsp)
{

}
/*
bool Reader::getFullMem(Tag &tag)
{
    return (getEPCMem(tag) && getUserMem(tag) && getReserveMem(tag));
}
*/
