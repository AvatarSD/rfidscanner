#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <QTimer>
#include <QSharedPointer>
#include "events.h"


/********************** Level 1(A) ***********************/

/*************** Interface ****************/

/******** ScannerPhy *******/
class ScannerPhy : public Eventful
{
    Q_OBJECT
public:
    ScannerPhy(QObject*parent=nullptr) : Eventful(parent){}
    virtual ~ScannerPhy(){}
    
signals:
    void inData(QByteArray data);
    void opened();
    void closed();
    
public slots:
    virtual void outData(QByteArray data)=0;
    virtual int open()=0;
    virtual void close()=0;
    virtual bool isOpen()=0;
    
};

#endif // TRANSPORT_H
