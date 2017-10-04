#include "transport.h"

PhyTransport::PhyTransport() : Eventful(nullptr)
{
    connect(&timer, SIGNAL(QTimer::timeout()),this,SLOT(run()));

    this->moveToThread(&thread);
    thread.start();
    timer.start(0);
}

PhyTransport::~PhyTransport()
{
    timer.stop();
    thread.quit();
    thread.wait();
}
