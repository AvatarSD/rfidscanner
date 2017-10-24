#ifndef CHINECOMPACTHIDSCANNER_H
#define CHINECOMPACTHIDSCANNER_H

#include "../c_reader.h"


/*********************** Basic ************************/

//struct UsbDevicePidVid{
//    uint16_t VendorID, ProductID;
//};

///*************** Transport ***************/
//class ChinaCompactHidTransport : public PhyTransport
//{
//    Q_OBJECT
//public:
//    ChinaCompactHidTransport(QSharedPointer<UsbDevicePidVid>);
//    virtual ~ChinaCompactHidTransport(){}

//public slots:
//    virtual void outData(QByteArray data);
//    virtual int open();
//    virtual void close();
//    virtual bool isOpen();

//protected slots:
//    virtual void run();

//private:
////    QSharedPointer<QUSB::Device> dev;
//    bool autoopenRequired;
//};

///**************** Protocol ****************/
//class ChinaCompactHidProtocol : public ReaderProtocol
//{
//public:
//    ChinaCompactHidProtocol();
//};

///*************** Reader ***************/
//class ChinaCompactHidScanner : public Reader
//{
//    Q_OBJECT
//public:
//    ChinaCompactHidScanner(QSharedPointer<UsbDevicePidVid> dev) :
//        Reader(QSharedPointer<ChinaCompactHidProtocol>(new ChinaCompactHidProtocol()),
//                   QSharedPointer<ChinaCompactHidTransport>(new ChinaCompactHidTransport(dev)))
//    {}
//};





#endif // CHINECOMPACTHIDSCANNER_H
