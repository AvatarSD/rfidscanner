#ifndef READER_H
#define READER_H

#include <QObject>
#include <QScopedPointer>
#include <QThread>

#include "a_transport.h"
#include "b_proto.h"



/**** General Scanner Settings ****/
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
/*class ScannerSettings
{

};*/




/********** Scanner *********/
class Scanner : public Eventful
{
    Q_OBJECT
public:
    enum ScannerStateEnum {
        DETHACHED,
        ATTACHED,
        READY
    };
    Q_ENUM(ScannerStateEnum)
    Scanner(ScannerProtocol * protocol, PhyTransport * transport,
           QObject * parent = nullptr);
    virtual ~Scanner();
public slots:
    ScannerStateEnum status() const;
    //return null-string if yes, otherwise return err string
    virtual QString isAddrValid(QString addr)=0; 
    virtual QStringList validAddrList()=0;
    virtual void attach(QString addr) = 0;
    virtual void detach() = 0;
    virtual void execute(QSharedPointer<ScannerRequest>) = 0;
signals:
    void executed(QSharedPointer<ScannerReply>);
    void statusChanged(ScannerStateEnum status);
    void validAddrListChanged(QStringList addrList);
protected:
    QScopedPointer<ScannerProtocol> protocol;
    QScopedPointer<PhyTransport> transport;
protected slots:
    void setStatus(ScannerStateEnum status);
    virtual void inData(QByteArray) = 0;
signals:
    void outData(QByteArray);
private:
    QThread thread;
    ScannerStateEnum m_status;
};


#endif // READER_H
