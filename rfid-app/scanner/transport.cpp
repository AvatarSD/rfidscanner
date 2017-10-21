#include "transport.h"

PhyTransport::PhyTransport() : Eventful(nullptr)
{
    connect(&timer, SIGNAL(QTimer::timeout()),this,SLOT(run()));


}

PhyTransport::~PhyTransport()
{

}
