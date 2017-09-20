#include "rfidmamanger.h"


/************* TagStatus *************/

QDateTime TagStatus::getLastVisiableTime() const
{
    return lastVisiableTime;
}

void TagStatus::setLastVisiableTime(const QDateTime &value)
{
    lastVisiableTime = value;
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

RFIDMamanger::RFIDMamanger()
{
    this->moveToThread(&thread);
    thread.start();
}

RFIDMamanger::~RFIDMamanger()
{
    thread.quit();
    thread.wait();
}


/******** SimpleRFIDMamanger ********/

SimpleRFIDMamanger::SimpleRFIDMamanger(QSharedPointer<RFIDReader> reader) :
    RFIDMamanger(), timer(this), reader(reader)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

SimpleRFIDMamanger::~SimpleRFIDMamanger()
{

}

void SimpleRFIDMamanger::run()
{
    timer.start(10);
}

void SimpleRFIDMamanger::stop()
{
    timer.stop();
}

void SimpleRFIDMamanger::onTimer()
{

}

