#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <QTimer>
#include <QSharedPointer>
#include "events.h"


class PhyTransport : public Eventful
{
    Q_OBJECT
public:
    PhyTransport(QObject*parent=nullptr) : Eventful(parent){}
    virtual ~PhyTransport(){}
    
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
