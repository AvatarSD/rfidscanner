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


/******** SimpleReaderManenger ********/

SimpleReaderManenger::SimpleReaderManenger(QSharedPointer<Reader> reader) :
    ReaderManenger(), timer(this), reader(reader)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

SimpleReaderManenger::~SimpleReaderManenger()
{

}

void SimpleReaderManenger::run()
{
    timer.start(10);
}

void SimpleReaderManenger::stop()
{
    timer.stop();
}

void SimpleReaderManenger::onTimer()
{

}

