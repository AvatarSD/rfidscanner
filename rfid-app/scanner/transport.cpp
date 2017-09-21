#include "transport.h"

PhyTransport::PhyTransport() : QObject(nullptr)
{
    this->moveToThread(&thread);
    thread.start();
}

PhyTransport::~PhyTransport()
{
    thread.quit();
    thread.wait();
}
