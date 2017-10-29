#ifndef READER_H
#define READER_H

#include <QObject>
#include <QScopedPointer>
#include <QThread>

#include "a_transport.h"
#include "b_commands.h"


/********************** Level 3(C) ***********************/

/*************** Interface ****************/

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
    Scanner(QObject * parent = nullptr);
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
protected slots:
    void setStatus(ScannerStateEnum status);
private:
    ScannerStateEnum m_status;
};


/************ Implementation **************/

/***** ScannerEmulator *****/
class ScannerEmulator : public Scanner
{
    Q_OBJECT
public:
    ScannerEmulator(QObject * parent=nullptr) : Scanner(parent){}
    virtual ~ScannerEmulator(){}
    
public slots:
    virtual QString isAddrValid(QString addr);
    virtual QStringList validAddrList();
    virtual void attach(QString addr);
    virtual void detach();
    virtual void execute(QSharedPointer<ScannerRequest>);
};

/********** Scanner *********/
class ScannerDet : public Scanner
{
    Q_OBJECT
public:
    ScannerDet(ScannerProtocol * protocol, ScannerPhy * transport,
               QObject * parent = nullptr) : Scanner(parent) {}
    virtual ~ScannerDet(){}
public slots:
    //return null-string if yes, otherwise return err string
    virtual QString isAddrValid(QString addr){}
    virtual QStringList validAddrList(){}
    virtual void attach(QString addr){}
    virtual void detach(){}
    virtual void execute(QSharedPointer<ScannerRequest>){}
    /*********************/
protected:
    QScopedPointer<ScannerProtocol> protocol;
    QScopedPointer<ScannerPhy> transport;
protected slots:
    virtual void inData(QByteArray){}
signals:
    void outData(QByteArray);
private:
    QThread thread;
};

#endif // READER_H
