#include "rfidmamanger.h"

RFIDMamanger::RFIDMamanger(RFIDReader *reader) :
    reader(reader)
{}

RFIDMamanger::~RFIDMamanger()
{
    delete  reader;
}

const RFIDMamanger::LogList &RFIDMamanger::getRuntimeList()
{
    return logs;
}
