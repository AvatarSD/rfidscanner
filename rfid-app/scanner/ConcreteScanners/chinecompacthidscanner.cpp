#include "chinecompacthidscanner.h"


ChinaCompactHidTransport::ChinaCompactHidTransport(QSharedPointer<QUSB::Device>)
{

}

void ChinaCompactHidTransport::outData(QByteArray data)
{

}

int ChinaCompactHidTransport::open()
{
    autoopenRequired = true;

}

void ChinaCompactHidTransport::close()
{
    autoopenRequired = false;

}

bool ChinaCompactHidTransport::isOpen()
{

}

void ChinaCompactHidTransport::run()
{

}
