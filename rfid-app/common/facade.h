#ifndef SCANNERFACADE_H
#define SCANNERFACADE_H

#include <QObject>

#include "common/system.h"
#include "common/logger.h"
#include "scanner/e_mamanger.h"
#include "network/d_client.h"

class ScannerFacade : public Eventful
{
    Q_OBJECT

    /*I*/ // Informations property, cannot be set
    /******************/
    /*N*/ // Need to set, otherwise value have a default value
    /******************/
    /*C*/ // Require re-create object(s)
    /*R*/ // Require re-connect.
    /******************/
    // All 'C' properties is 'R'. All 'C' or 'R' properties
    // is hold by ScannerFacade internal. It's require for 
    // saving it for next creation/connection procedure.
    // Otherwise it will r/w from appropriate object.

    /***** LOGGER ****/
    /*   */ Q_PROPERTY(QString     logfile         READ logfile         WRITE setLogfile         NOTIFY logfileChanged)
    /***** NETWORK ****/
    /* I */ Q_PROPERTY(NetSettStat isReady         READ isReady                                  NOTIFY isReadyChanged)
    /* I */ Q_PROPERTY(bool        isReconRequire  READ isReconRequire                           NOTIFY isReconRequireChanged)
    /*   */     /* socket */
    /*  C*/ Q_PROPERTY(ClientType  clientType      READ clientType      WRITE setClientType      NOTIFY clientTypeChanged)
    /*  C*/ Q_PROPERTY(SocketType  socket          READ socket          WRITE setSocket          NOTIFY socketChanged)
    /*N R*/ Q_PROPERTY(QString     server          READ server          WRITE setServer          NOTIFY serverChanged)
    /*N R*/ Q_PROPERTY(quint16     port            READ port            WRITE setPort            NOTIFY portChanged)
    /*  R*/ //Q_PROPERTY(QString     sslKey          READ sslKey          WRITE setSslKey          NOTIFY sslKeyChanged)
    /*  R*/ //Q_PROPERTY(QString     sslCert         READ sslCert         WRITE setSslCert         NOTIFY sslCertChanged)
    /*   */     /* proto */
    /*  C*/ Q_PROPERTY(MsgBound    msgBoundaries   READ msgBoundaries   WRITE setMsgBoundaries   NOTIFY msgBoundariesChanged)
    /*  C*/ Q_PROPERTY(QString     startSqns       READ startSqns       WRITE setStartSqns       NOTIFY startSqnsChanged)
    /*  C*/ Q_PROPERTY(QString     tailSqns        READ tailSqns        WRITE setTailSqns        NOTIFY tailSqnsChanged)
    /*   */     /* auth */
    /*N R*/ Q_PROPERTY(QString     username        READ username        WRITE setUsername        NOTIFY usernameChanged)
    /*N R*/ Q_PROPERTY(QString     password        READ password        WRITE setPassword        NOTIFY passwordChanged)
    /* I */ Q_PROPERTY(NetStateEnum netState       READ netState                                 NOTIFY netStateChanged)
    /* I */ Q_PROPERTY(QString     netStateMsg     READ netStateMsg                              NOTIFY netStateMsgChanged)
    /*  C*/ //Q_PROPERTY(AuthType    authType        READ authType        /*WRITE setAuthType*/      /*NOTIFY authTypeChanged*/)
    /*   */     /* commander */
    /*   */ Q_PROPERTY(NetModeEnum mode            READ mode            WRITE setMode            NOTIFY modeChanged)
    /*   */ Q_PROPERTY(uint        msgTxRepeatSec  READ msgTxRepeatSec  WRITE setMsgTxRepeatSec  NOTIFY msgTxRepeatSecChanged)
    /*   */ Q_PROPERTY(uint        msgMaxTxAtempt  READ msgMaxTxAtempt  WRITE setMsgMaxTxAtempt  NOTIFY msgMaxTxAtemptChanged)
    /*   */ Q_PROPERTY(qint32      msgInspectMsec  READ msgInspectMsec  WRITE setMsgInspectMsec  NOTIFY msgInspectMsecChanged)
    /***** SCANNER ****/
    /* I */ //Q_PROPERTY(bool isScannerReady READ isScannerReady WRITE setIsScannerReady NOTIFY isScannerReadyChanged)
    /* I */ //Q_PROPERTY(bool isScannerReconReq READ isScannerReconReq WRITE setIsScannerReconReq NOTIFY isScannerReconReqChanged)
    /*   */     /* phy */
    /*N C*/ //Q_PROPERTY(ScannerType  scannerType      READ scannerType      /*WRITE setScannerType*/    /*NOTIFY scannerTypeChanged*/)
    /*N C*/ //Q_PROPERTY(QString     scannerAddr      READ scannerAddr      /*WRITE setScannerAddr*/    /*NOTIFY scannerAddrChanged*/)
    /* I */ //Q_PROPERTY(ScannerStateEnum scannerState READ scannerState WRITE setScannerState NOTIFY scannerStateChanged)
    /*   */     /* mananger */
    /* I */ //Q_PROPERTY(QVariant    field           READ field                                    NOTIFY fieldChanged)
    /*   */ //Q_PROPERTY(uint        scanPeriodMsec  READ scanPeriodMsec  WRITE setScanPeriodMsec  NOTIFY scanPeriodMsecChanged)
    /*   */ //Q_PROPERTY(uint        maxUnreadMsec   READ maxUnreadMsec   WRITE setMaxPeriodMsec   NOTIFY maxUnreadMsecChanged)
    /*   */ //Q_PROPERTY(uint        maxUnreadPcnt   READ maxUnreadPcnt   WRITE setMaxUnreadPcnt   NOTIFY maxUnreadPcntChanged)
    /***** SYSTEM ****/
    /* I */ //Q_PROPERTY(QVariantMap wlanStatus      READ wlanStatus                               NOTIFY wlanStatusChanged)
    /* I */ //Q_PROPERTY(QVariantMap wlans           READ wlans                                    NOTIFY wlansChanged)

public:
    typedef NetClientStateClass::NetClientStateEnum NetStateEnum;
    typedef NetClientModeClass::NetClientModeEnum NetModeEnum;

    enum NetSettStat{
        OK = 0,
        NO_SERV = 0b0001,
        NO_PORT = 0b0010,
        NO_USER = 0b0100,
        NO_PASS = 0b1000
    };
    enum ClientType{V1Basic};
    enum SocketType{TCP, SSL};
    enum MsgBound{SIMPLE, BOUND_V1};
    enum AuthType{JSON, BASE64};
    enum ScannerType{ADS_USB, LINK_SPRITE};

    Q_ENUM(NetSettStat)
    Q_ENUM(ClientType)
    Q_ENUM(SocketType)
    Q_ENUM(MsgBound)
    Q_ENUM(AuthType)
    Q_ENUM(ScannerType)
    
    /*************/
    
    typedef Scanner::ScannerStateEnum ScannerStateEnum;

    explicit ScannerFacade(QObject*parent=nullptr);
    ~ScannerFacade();

public slots:
    void connectToServer();
    void disconnectFromServer();
    void connectToScanner();
    void disconnectFromScanner();
    //void connectToWlan(QString ssid, QString psswd);
    //void disconnectFromWlan();

public:
    /* net: is reconnection required */
    bool isReconRequire() const;
    /* net: is all required fields are fill */
    NetSettStat isReady();
    /* net info */
    NetStateEnum netState() const;
    QString netStateMsg() const;
    /* net re-create */
    ClientType clientType() const;
    SocketType socket() const;
    MsgBound msgBoundaries() const;
    QString startSqns() const;
    QString tailSqns() const;
    /* net re-connect */
    QString server() const;
    quint16 port() const;
    QString username() const;
    QString password() const;
    /* net: realtime */
    NetModeEnum mode() const;
    uint msgTxRepeatSec() const;
    uint msgMaxTxAtempt() const;
    qint32 msgInspectMsec() const;
    /* by mutex */
    QString logfile() const;


    
    
public slots:
    /* net: re-create */
    void setClientType(ClientType clientType);
    void setSocket(SocketType socket);
    void setMsgBoundaries(MsgBound msgBoundaries);
    void setStartSqns(QString startSqns);
    void setTailSqns(QString tailSqns);
    /* net: re-connect */
    void setServer(QString server);
    void setPort(quint16 port);
    void setUsername(QString username);
    void setPassword(QString password);
    /* net: realtime */
    void setMode(NetModeEnum mode);
    void setMsgTxRepeatSec(uint msgTxRepeatSec);
    void setMsgMaxTxAtempt(uint msgMaxTxAtempt);
    void setMsgInspectMsec(qint32 msgInspectMsec);
    /* by internal mutex */
    void setLogfile(QString logfile);


    
    
signals:
    /* net: is reconnection required */
    void isReconRequireChanged(bool isReconRequire);
    /* net: is all required fields are fill */
    void isReadyChanged(NetSettStat isReady);
    /* net: info */
    void netStateChanged(NetStateEnum netState);
    void netStateMsgChanged(QString stateMsg);
    /* net: re-create */
    void clientTypeChanged(ClientType clientType);
    void socketChanged(SocketType socket);
    void msgBoundariesChanged(MsgBound msgBoundaries);
    void startSqnsChanged(QString startSqns);
    void tailSqnsChanged(QString tailSqns);
    /* net: re-connect */
    void serverChanged(QString server);
    void portChanged(quint16 port);
    void usernameChanged(QString username);
    void passwordChanged(QString password);
    /* net: realtime */
    void modeChanged(NetModeEnum mode);
    void msgTxRepeatSecChanged(uint msgTxRepeatSec);
    void msgMaxTxAtemptChanged(uint msgMaxTxAtempt);
    void msgInspectMsecChanged(qint32 msgInspectMsec);
    /* by internal mutex */
    void logfileChanged(QString logfile);
    
    
private slots:
    void netStateChangedHandler(const NetClientState *state);
private:
    void putNetStatusToLog(NetSettStat isReady);
    void setNetReCreateRequire(bool require);
    void setNetReConectRequire(bool require);
    bool netCreareProcedure();
    bool netConnectProcedure();

private:
    QThread logManengerThread;
    QThread netManengerThread;
    QThread scannerManengerThread;
//    QThread sysManengerThread;

    /* static */
    QScopedPointer<Logger> logger;
//    QScopedPointer<System> system;
    /* dynamic */
    QScopedPointer<NetClient> network;
    QScopedPointer<ScannerManenger> scanner;

    bool netReCreateRequire;
    bool netReConectRequire;


private:
    /* net re-create */
    ClientType m_clientType;
    SocketType m_socket;
    MsgBound m_msgBoundaries;
    QString m_startSqns;
    QString m_tailSqns;
    /* net re-connect */
    QString m_server;
    quint16 m_port;
    QString m_username;
    QString m_password;
    /* realtime (save for reconnections)*/
    NetModeEnum m_mode;
    uint m_msgTxRepeatSec;
    uint m_msgMaxTxAtempt;
    qint32 m_msgInspectMsec;
    
};

#endif // SCANNERFACADE_H
