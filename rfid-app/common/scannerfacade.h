#ifndef SCANNERFACADE_H
#define SCANNERFACADE_H

#include <QObject>

#include "common/system.h"
#include "common/logger.h"
#include "scanner/rfidmamanger.h"
#include "network/networkclient.h"

class ScannerFacade : public Eventful
{
    Q_OBJECT
    Q_ENUMS(Mode)
    Q_ENUMS(Reader)
    Q_ENUMS(MsgBound)
    Q_ENUMS(State)
    Q_ENUMS(AuthType)
    Q_ENUMS(Socket)

    /*N*/ // Need
    /*O*/ // Optional
    /*I*/ // Informations
    /*D*/ // Default value
    /*R*/ // Runtime motifiting
    /*C*/ // Re-Create object(s)

    /*     */     /**** net connectio ****/
    /*N C  */ Q_PROPERTY(Socket   socket          READ socket          /*WRITE setSocket*/      /*NOTIFY socketChanged*/)
    /*N R  */ Q_PROPERTY(QString  server          READ server          WRITE setServer          NOTIFY serverChanged)
    /*N R  */ Q_PROPERTY(quint16  port            READ port            WRITE setPort            NOTIFY portChanged)
    /*O R  */ //Q_PROPERTY(QString  sslKey          READ sslKey          WRITE setSslKey          NOTIFY sslKeyChanged)
    /*O R  */ //Q_PROPERTY(QString  sslCert         READ sslCert         WRITE setSslCert         NOTIFY sslCertChanged)
    /*     */     /**** net proto ****/
    /*O C D*/ //Q_PROPERTY(MsgBound msgBoundaries   READ msgBoundaries   /*WRITE setMsgBoundaries*/ /*NOTIFY msgBoundariesChanged*/)
    /*O R D*/ Q_PROPERTY(QString  startSqns       READ startSqns       WRITE setStartSqns       NOTIFY startSqnsChanged)
    /*O R D*/ Q_PROPERTY(QString  tailSqns        READ tailSqns        WRITE setTailSqns        NOTIFY tailSqnsChanged)
    /*     */     /**** auth ****/
    /*N R  */ Q_PROPERTY(QString  username        READ username        WRITE setUsername        NOTIFY usernameChanged)
    /*N R  */ Q_PROPERTY(QString  password        READ password        WRITE setPassword        NOTIFY passwordChanged)
    /* I   */ Q_PROPERTY(State    state           READ state                                    NOTIFY stateChanged)
    /* I   */ Q_PROPERTY(QString  stateMsg        READ stateMsg                                 NOTIFY stateMsgChanged)
    /*O C D*/ //Q_PROPERTY(AuthType authType        READ authType        /*WRITE setAuthType*/      /*NOTIFY authTypeChanged*/)
    /*     */     /**** net commander ****/
    /*O R D*/ Q_PROPERTY(Mode     mode            READ mode            WRITE setMode            NOTIFY modeChanged)
    /*O R D*/ Q_PROPERTY(uint     msgTxRepeatSec  READ msgTxRepeatSec  WRITE setMsgTxRepeatSec  NOTIFY msgTxRepeatSecChanged)
    /*O R D*/ Q_PROPERTY(uint     msgMaxTxAtempt  READ msgMaxTxAtempt  WRITE setMsgMaxTxAtempt  NOTIFY msgMaxTxAtemptChanged)
    /*O R D*/ Q_PROPERTY(qint32   msgInspectMsec  READ msgInspectMsec  WRITE setMsgInspectMsec  NOTIFY msgInspectMsecChanged)
    /*     */     /**** reader ****/
    /*N C  */ //Q_PROPERTY(Reader   readerType      READ readerType      /*WRITE setReaderType*/    /*NOTIFY readerTypeChanged*/)
    /*N C  */ //Q_PROPERTY(QString  readerAddr      READ readerAddr      /*WRITE setReaderAddr*/    /*NOTIFY readerAddrChanged*/)
    /* I   */ //Q_PROPERTY(QVariant field           READ field                                    NOTIFY fieldChanged)
    /*O R D*/ //Q_PROPERTY(uint     scanPeriodMsec  READ scanPeriodMsec  WRITE setScanPeriodMsec  NOTIFY scanPeriodMsecChanged)
    /*O R D*/ //Q_PROPERTY(uint     maxUnreadMsec   READ maxUnreadMsec   WRITE setMaxPeriodMsec   NOTIFY maxUnreadMsecChanged)
    /*O R D*/ //Q_PROPERTY(uint     maxUnreadPcnt   READ maxUnreadPcnt   WRITE setMaxUnreadPcnt   NOTIFY maxUnreadPcntChanged)
    /*     */     /**** system ****/
    /*O R D*/ Q_PROPERTY(QString  logfile         READ logfile         WRITE setLogfile         NOTIFY logfileChanged)
    /*O R  */// Q_PROPERTY(QString  wlanSsid        READ ssid            /*WRITE setSsid*/        NOTIFY ssidChanged)
    /*O R  */ //Q_PROPERTY(QString  wlanPass        READ wlanPass        /*WRITE setWlanPass*/    NOTIFY wlanPassChanged)





public:
    typedef BasicV1Client::WorkMode Mode;
    typedef NetCommanderState::States State;

    enum Reader{ADS_USB, LINK_SPRITE};
    enum MsgBound{SIMPLE, BOUND_V1};
    enum Socket{TCP, SSL};
    enum AuthType{JSON, BASE64};

    ScannerFacade();


};

#endif // SCANNERFACADE_H
