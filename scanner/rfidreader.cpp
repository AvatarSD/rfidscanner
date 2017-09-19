#include "rfidreader.h"

RFIDReader::RFIDReader(QSharedPointer<Protocol> prot, QSharedPointer<Transport> trnsp):
    protocol(prot), transport(trnsp)
{

}
/*
bool RFIDReader::getFullMem(Tag &tag)
{
    return (getEPCMem(tag) && getUserMem(tag) && getReserveMem(tag));
}
*/
