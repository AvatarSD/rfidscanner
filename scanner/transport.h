#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <QThread>
#include <QTimer>
#include <QSharedPointer>
#include "events.h"


class PhyTransport : public QObject
{
    Q_OBJECT
public:
    PhyTransport();
    virtual ~PhyTransport();

signals:
    void inData(QByteArray data);
    void opened();
    void closed();
    void otherEvent(QSharedPointer<InfoEvent>);

public slots:
    virtual int open()=0;
    virtual void close()=0;
    virtual void outData(QByteArray data)=0;
    virtual bool isOpen();

protected:


private:
    QThread thread;
};

#endif // TRANSPORT_H
