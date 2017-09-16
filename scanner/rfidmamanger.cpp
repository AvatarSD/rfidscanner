#include "rfidmamanger.h"

QDateTime TagStatus::getLastVisiagleTime() const
{
    return lastVisiagleTime;
}

void TagStatus::setLastVisiagleTime(const QDateTime &value)
{
    lastVisiagleTime = value;
}

uint TagStatus::getReadCount() const
{
    return readCount;
}

void TagStatus::setReadCount(const uint &value)
{
    readCount = value;
}

int TagStatus::getRssi() const
{
    return rssi;
}

void TagStatus::setRssi(int value)
{
    rssi = value;
}


/*********** RFIDManamger ***********/

RFIDMamanger::RFIDMamanger(QSharedPointer<RFIDReader> reader) :
    reader(reader)
{
    thread = new QThread;
    this->moveToThread(thread);
}

RFIDMamanger::~RFIDMamanger()
{
    delete thread;
}


/******** SimpleRFIDMamanger ********/

void SimpleRFIDMamanger::run()
{

}
