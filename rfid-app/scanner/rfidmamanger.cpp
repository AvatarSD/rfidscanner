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


/*********** ReaderManenger ***********/

ReaderManenger::ReaderManenger()
{
//    this->moveToThread(&thread);
    thread.setObjectName("Reader Phy");
    thread.start();
}

ReaderManenger::~ReaderManenger()
{
    thread.quit();
    thread.wait();
}


/******** ReaderManengerSimple ********/

ReaderManengerSimple::ReaderManengerSimple(QSharedPointer<Reader> reader) :
    ReaderManenger(), timer(this), reader(reader)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

ReaderManengerSimple::~ReaderManengerSimple()
{

}

void ReaderManengerSimple::run()
{
    timer.start(10);
}

void ReaderManengerSimple::stop()
{
    timer.stop();
}

void ReaderManengerSimple::onTimer()
{

}

