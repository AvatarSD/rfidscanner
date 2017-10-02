#ifndef CHINECOMPACTHIDSCANNER_H
#define CHINECOMPACTHIDSCANNER_H

#include "../rfidreader.h"
#include "device.h"
#include <QSharedPointer>


/*********************** Basic ************************/

/*************** Transport ***************/
class ChinaCompactHidTransport : public PhyTransport
{
    Q_OBJECT
public:
    ChinaCompactHidTransport(QSharedPointer<QUSB::Device>);
    virtual ~ChinaCompactHidTransport();

public slots:
    virtual void outData(QByteArray data);
    virtual int open();
    virtual void close();
    virtual bool isOpen();

protected slots:
    virtual void run();

private:
//    QSharedPointer<QUSB::Device> dev;
    bool autoopenRequired;
};

/**************** Protocol ****************/
class ChinaCompactHidProtocol : public Protocol
{
public:
    ChinaCompactHidProtocol();
};

/*************** RFIDReader ***************/
class ChinaCompactHidScanner : public RFIDReader
{
    Q_OBJECT
public:
    ChinaCompactHidScanner(QSharedPointer<QUSB::Device> dev) :
        RFIDReader(new ChinaCompactHidProtocol(),
                   new ChinaCompactHidTransport(dev))
    {}
};





#endif // CHINECOMPACTHIDSCANNER_H
