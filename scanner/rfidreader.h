#ifndef RFIDREADER_H
#define RFIDREADER_H

#include "types.h"
#include "events.h"
#include "transport.h"
#include "rfidprotocol.h"

#include <QObject>
#include <QSharedPointer>


/**** General Reader Settings ****/
typedef int dBm;
typedef int Frequency;
typedef std::list<Frequency> WorkingFrequencies;
enum Modulation{
    FM0,
    Miller1,
    Miller2,
    Miller4,
    Miller8
};
enum Target{
    Single, Dual
};
enum Session{
    S0 = 0, S1 = 1, S2 = 2, S3 = 3
};
typedef uint QValue;

/************/
class RFIDReaderSettings
{

};




/**** RFID Reader ****/
class RFIDReader : public QObject
{
    Q_OBJECT
public:
    RFIDReader(QSharedPointer<Protocol> prot, QSharedPointer<Transport> trnsp);
    virtual ~RFIDReader(){}

public slots:
    virtual QSharedPointer<ScannerReply> execBlock(QSharedPointer<ScannerRequest> cmd);
    virtual void execAsync(QSharedPointer<ScannerRequest> cmd);

signals:
    void executed(QSharedPointer<ScannerReply> cmd);
    void otherEvent(QSharedPointer<InfoEvent> event);

protected:
    QSharedPointer<Protocol> protocol;
    QSharedPointer<Transport> transport;
//    QQueue<QSharedPointer<ScannerRequest>> msgQueue;

protected slots:
    virtual void read(QByteArray data);
signals:
    void write(QByteArray data);

};


#endif // RFIDREADER_H
