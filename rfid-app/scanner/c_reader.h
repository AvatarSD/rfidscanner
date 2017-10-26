#ifndef READER_H
#define READER_H

#include <QObject>
#include <QScopedPointer>
#include <QThread>

#include "a_transport.h"
#include "b_proto.h"



/**** General Reader Settings ****/
/*typedef int dBm;
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
typedef uint QValue;*/

/************/
/*class ReaderSettings
{

};*/




/********** Reader *********/
class Reader : public Eventful
{
    Q_OBJECT
public:
    enum ReaderStateEnum {
        DETHACHED,
        ATTACHED,
        READY
    };
    Q_ENUM(ReaderStatus)
    Reader(ReaderProtocol * protocol, PhyTransport * transport,
           QObject * parent = nullptr);
    virtual ~Reader();
public slots:
    ReaderStateEnum status() const;
    virtual void attach(QString addr) = 0;
    virtual void detach() = 0;
    virtual void execute(QSharedPointer<ScannerRequest>) = 0;
signals:
    void executed(QSharedPointer<ScannerReply>);
    void statusChanged(ReaderStateEnum status);
protected:
    QScopedPointer<ReaderProtocol> protocol;
    QScopedPointer<PhyTransport> transport;
protected slots:
    void setStatus(ReaderStateEnum status);
    virtual void inData(QByteArray) = 0;
signals:
    void outData(QByteArray);
private:
    QThread thread;
    ReaderStateEnum m_status;
};


#endif // READER_H
