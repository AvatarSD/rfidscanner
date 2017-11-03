#include "facade.h"
#include <QtQml>
#include <QMetaObject>
#include <QMetaEnum>


ScannerFacade::ScannerFacade(QObject *parent) : Eventful(parent),
    logManengerThread(this), logger(new Logger),
    netManengerThread(this), scannerManengerThread(this)// sysManengerThread(this);
{    
    /*** register for qml usage ***/
    qmlRegisterType<ScannerFacade>("ScannerMainElements",1,0,"Facade");
    
    /************** Logger ****************/
    logManengerThread.setObjectName("Event Manenger");
    logger->moveToThread(&logManengerThread);
    connect(this, &ScannerFacade::sysEvent,
            logger.data(), &Logger::sysEventIn,
            Qt::QueuedConnection);
    logManengerThread.start();
    
    /************** Network ***************/
    /* register meta-types */
    qRegisterMetaType<NetStateEnum>("NetStateEnum");
    qRegisterMetaType<NetModeEnum>("NetModeEnum");
    /* register for qml usage */
    qmlRegisterType<NetClientModeClass>("ScannerMainElements",1,0,"NetMode");
    qmlRegisterType<NetClientStateClass>("ScannerMainElements",1,0,"NetState");
    /* todo: test without this */
    //    netReCreateRequire = true;
    //    netReConectRequire = true;
    /* default values */
    m_clientType = ClientType::V1Basic;
    m_socket = SocketType::TCP;
    m_msgBoundaries = MsgBound::BOUND_V1;
    m_startSqns = QStringLiteral("$SD#");
    m_tailSqns = QStringLiteral("\r\n\r\n");
    //m_authType = AuthType::JSON;
    /* setup net thread(call moveToThread from creating procedure) */
    netManengerThread.setObjectName("Net Manenger");
    netManengerThread.start();
    /* create req obj*/
    netCreareProcedure();
    
    /************** System ****************/
    //sysManengerThread.setObjectName("System Manenger");
    //sysManengerThread.start();
    
    /************** Scanner ***************/
    /* register meta-types */
    
    /* register for qml usage */
    
    /* default values */
    m_scannerType = ScannerType::EMULATOR;
    m_scannerAddr = QStringLiteral("Simulator");
    /* setup net thread(call moveToThread from creating procedure) */
    scannerManengerThread.setObjectName("Scanner Manenger");
    scannerManengerThread.start();
    /* create req obj*/
    scannerCreareProcedure();
    
}
ScannerFacade::~ScannerFacade()
{
    void disconnectFromScanner();
    disconnectFromServer();
    //void disconnectFromWlan();
    
    //sysManengerThread.quit();
    scannerManengerThread.quit();
    netManengerThread.quit();
    logManengerThread.quit();
    
    //sysManengerThread.wait();
    scannerManengerThread.wait();
    netManengerThread.wait();
    logManengerThread.wait();
}


/***************** LOGGER *****************/
QString ScannerFacade::logfile() const{
    return logger->getLogfilePath();
}
void ScannerFacade::setLogfile(QString logfile){
    if (this->logfile() != logfile)
        logger->setLogfilePath(logfile);
    emit logfileChanged(logfile);
}


/***************** NETWORK ****************/
/***** Network: Service ****/
/* facade control */
void ScannerFacade::connectToServer()
{
    /* close old socket */
    if(!network.isNull()){
        disconnectFromServer();
        while(*network->state() != NetStateEnum::DISCONNECTED)
            QObject::thread()->yieldCurrentThread();
    }
    /* re-create procedure */
    if(netReCreateRequire || network.isNull())
        if(!netCreareProcedure())
            return;
    /* re-connect procedure */
    if(netReConectRequire)
        if(!netConnectProcedure())
            return;
    /* call connectToHost */
    QMetaObject::invokeMethod(network.data(), "start", Qt::QueuedConnection);
}
void ScannerFacade::disconnectFromServer()
{
    if(!network.isNull())
        QMetaObject::invokeMethod(network.data(), "stop", Qt::QueuedConnection);
}
/* facade info */
bool ScannerFacade::isReconRequire() const{
    return netReCreateRequire || netReConectRequire;
}
ScannerFacade::NetSettState ScannerFacade::netSettState() {
    uint8_t stat = OK;
    if(m_server.isEmpty())
        stat |= NO_SERV;
    if(m_port == 0)
        stat |= NO_PORT;
    if(m_username.isEmpty())
        stat |= NO_USER;
    if(m_password.isEmpty())
        stat |= NO_PASS;
    return (NetSettState)stat;
}
/* private */
void ScannerFacade::netStateChangedHandler(const NetClientState *state){
    emit netStateChanged(state->stateEnum());
    emit netStateMsgChanged(state->stateMessage());
}
void ScannerFacade::chkNetSettState(NetSettState stat){
    putNetSettStateToLog(stat);
    emit netSettStateChanged(stat);
}
void ScannerFacade::putNetSettStateToLog(ScannerFacade::NetSettState stat){
    if(stat&NO_SERV)
        emit sysEvent(QSharedPointer<Event> (
                          new SystemEvent(SystemEvent::ERROR,
                                          SystemEvent::IDs::FACADE_STATUS,
                                          QStringLiteral("Server address is not set."))));
    if(stat&NO_PORT)
        emit sysEvent(QSharedPointer<Event> (
                          new SystemEvent(SystemEvent::ERROR,
                                          SystemEvent::IDs::FACADE_STATUS,
                                          QStringLiteral("Server port is 0."))));
    if(stat&NO_USER)
        emit sysEvent(QSharedPointer<Event> (
                          new SystemEvent(SystemEvent::WARNING,
                                          SystemEvent::IDs::FACADE_STATUS,
                                          QStringLiteral("Username is not set. "
                                                         "Connection without login."))));
    if(stat&NO_PASS)
        emit sysEvent(QSharedPointer<Event> (
                          new SystemEvent(SystemEvent::WARNING,
                                          SystemEvent::IDs::FACADE_STATUS,
                                          QStringLiteral("Password is not set. "
                                                         "Connection without login."))));
}
void ScannerFacade::setNetReCreateRequire(bool require){
    if(netReCreateRequire == require)
        return;
    netReCreateRequire = require;
    emit isReconRequireChanged(isReconRequire());
}
void ScannerFacade::setNetReConectRequire(bool require){
    emit netSettStateChanged(netSettState());
    if(netReConectRequire == require)
        return;
    netReConectRequire = require;
    emit isReconRequireChanged(isReconRequire());
}
bool ScannerFacade::netCreareProcedure()
{
    /* create new phy */
    NetPhy * phy = nullptr;
    switch (m_socket) { /*//todo
    case Socket::TCP:
        phy = new NetPhyTcp;
        break;
    case Socket::SSL:
        phy = new NetPhySsl;
        break;*/
    default:
        phy = new NetPhyTcp;
        break; }
    /* create new protocol */
    NetProtocol * proto = nullptr;
    switch (m_msgBoundaries) {/* //todo
    case MsgBound::BOUND_V1:
        proto = new NetProtocolV1Format(NetProtocolBound(m_startSqns, m_tailSqns));
        break;
    case MsgBound::SIMPLE:
        proto = new NetProtocolSimple(NetProtocolBound(m_startSqns, m_tailSqns));
        break;*/
    default:
        proto = new NetProtocolV1Format(
                    NetProtocolBound(
                        m_startSqns.toUtf8(),
                        m_tailSqns.toUtf8()));
        break; }
    /* create new client */
    NetClient * client = nullptr;
    switch (m_clientType) {
    case ClientType::V1Basic:
    default:
        client = new NetClientV1Basic(phy, proto);
        break;
    }    
    /* save settings */
    NetModeEnum mod; uint mtr, mta; qint32 mip = -666;
    if(!network.isNull()){
        mod = mode();
        mtr = msgTxRepeatSec();
        mta = msgMaxTxAtempt();
        mip = msgInspectMsec();
    }
    /* pun created object in member poiner */
    network.reset(client);
    /* move to net thread */
    network->moveToThread(&this->netManengerThread);
    /* connections */
    QObject::connect(network.data(), SIGNAL(sysEvent(QSharedPointer<Event>)),
                     logger.data(), SLOT(sysEventIn(QSharedPointer<Event>)),
                     Qt::QueuedConnection);
    QObject::connect(logger.data(), SIGNAL(netEventOut(QSharedPointer<Event>)),
                     network.data(),SLOT(netEventIn(QSharedPointer<Event>)),
                     Qt::QueuedConnection);
    QObject::connect(network.data(), SIGNAL(stateChanged(const NetClientState*)),
                     this, SLOT(netStateChangedHandler(const NetClientState*)),
                     Qt::QueuedConnection);
    /* load settings */
    if(mip != -666){
        network->setMode(mod);
        network->setMsgTransmitRepeatSec(mtr);
        network->setMsgMaxAtemptToDelete(mta);
        network->setMsgInspectPeriodMsec(mip);
    }
    
    /* reset re-create flag */
    setNetReConectRequire(true);
    setNetReCreateRequire(false);
    return true;
}
bool ScannerFacade::netConnectProcedure()
{
    /* check server, port, user and pass */
    auto ret = netSettState();
    chkNetSettState(ret);
    if(ret&NO_SERV || ret&NO_PORT)
        return false;
    /* set server and port */
    NetPoint np(m_server, m_port);
    QMetaObject::invokeMethod(network.data(),"setAddr", Qt::QueuedConnection,
                              Q_ARG(NetPoint, np));
    /* set user and pass */
    QAuthenticator auth;
    auth.setUser(m_username);
    auth.setPassword(m_password);
    QMetaObject::invokeMethod(network.data(),"setAuth", Qt::QueuedConnection,
                              Q_ARG(QAuthenticator, auth));
    /* reset re-connect flag */
    setNetReConectRequire(false);
    return true;
}

/**** Network: Settings ****/
/* internal info */
ScannerFacade::NetStateEnum ScannerFacade::netState() const{
    return network->state()->stateEnum();
}
QString ScannerFacade::netStateMsg() const{
    return network->state()->stateMessage();
}
/* re-creation settings */
ScannerFacade::ClientType ScannerFacade::clientType() const{
    return m_clientType;
}
ScannerFacade::SocketType ScannerFacade::socket() const{
    return m_socket;
}
ScannerFacade::MsgBound ScannerFacade::msgBoundaries() const{
    return m_msgBoundaries;
}
QString ScannerFacade::startSqns() const{
    return m_startSqns;
}
QString ScannerFacade::tailSqns() const{
    return m_tailSqns;
}
void ScannerFacade::setClientType(ScannerFacade::ClientType clientType){
    if (m_clientType == clientType)
        return;
    setNetReCreateRequire(true);
    m_clientType = clientType;
    emit clientTypeChanged(m_clientType);
}
void ScannerFacade::setSocket(ScannerFacade::SocketType socket){
    if (m_socket == socket)
        return;
    setNetReCreateRequire(true);
    m_socket = socket;
    emit socketChanged(m_socket);
}
void ScannerFacade::setMsgBoundaries(ScannerFacade::MsgBound msgBoundaries){
    if (m_msgBoundaries == msgBoundaries)
        return;
    setNetReCreateRequire(true);
    m_msgBoundaries = msgBoundaries;
    emit msgBoundariesChanged(m_msgBoundaries);
}
void ScannerFacade::setStartSqns(QString startSqns){
    if (m_startSqns == startSqns)
        return;
    setNetReCreateRequire(true);
    m_startSqns = startSqns;
    emit startSqnsChanged(m_startSqns);
}
void ScannerFacade::setTailSqns(QString tailSqns){
    if (m_tailSqns == tailSqns)
        return;
    setNetReCreateRequire(true);
    m_tailSqns = tailSqns;
    emit tailSqnsChanged(m_tailSqns);
}
/* re-connection settings */
QString ScannerFacade::server() const{
    return m_server;
}
quint16 ScannerFacade::port() const{
    return m_port;
}
QString ScannerFacade::username() const{
    return m_username;
}
QString ScannerFacade::password() const{
    return m_password;
}
void ScannerFacade::setServer(QString server){
    if (m_server == server)
        return;
    m_server = server;
    setNetReConectRequire(true);
    emit serverChanged(m_server); 
}
void ScannerFacade::setPort(quint16 port){
    if (m_port == port)
        return;
    m_port = port;
    setNetReConectRequire(true);
    emit portChanged(m_port);
}
void ScannerFacade::setUsername(QString username){
    if (m_username == username)
        return;
    m_username = username;
    setNetReConectRequire(true);
    emit usernameChanged(m_username);
}
void ScannerFacade::setPassword(QString password){
    if (m_password == password)
        return;
    m_password = password;
    setNetReConectRequire(true);
    emit passwordChanged(m_password);
}
/* direct settings */
ScannerFacade::NetModeEnum ScannerFacade::mode() const{
    return network->getMode();
}
uint ScannerFacade::msgTxRepeatSec() const{
    return network->getMsgTransmitRepeatSec();
}
uint ScannerFacade::msgMaxTxAtempt() const{
    return network->getMsgMaxAtemptToDelete();
}
qint32 ScannerFacade::msgInspectMsec() const{
    return network->getMsgInspectPeriodMsec();
}
void ScannerFacade::setMode(ScannerFacade::NetModeEnum mode){
    if (this->mode() != mode)
        QMetaObject::invokeMethod(network.data(),"setMode", 
                                  Qt::QueuedConnection, Q_ARG(NetModeEnum, mode));
    emit modeChanged(mode);
}
void ScannerFacade::setMsgTxRepeatSec(uint msgTxRepeatSec){
    if (this->msgTxRepeatSec() != msgTxRepeatSec)
        QMetaObject::invokeMethod(network.data(),"setMsgTransmitRepeatSec", 
                                  Qt::QueuedConnection, Q_ARG(uint, msgTxRepeatSec));
    emit msgTxRepeatSecChanged(msgTxRepeatSec);
}
void ScannerFacade::setMsgMaxTxAtempt(uint msgMaxTxAtempt){
    if (this->msgMaxTxAtempt() != msgMaxTxAtempt)
        QMetaObject::invokeMethod(network.data(),"setMsgMaxAtemptToDelete", 
                                  Qt::QueuedConnection, Q_ARG(uint, msgMaxTxAtempt));
    emit msgMaxTxAtemptChanged(msgMaxTxAtempt);
}
void ScannerFacade::setMsgInspectMsec(qint32 msgInspectMsec){
    if (this->msgInspectMsec() != msgInspectMsec)
        QMetaObject::invokeMethod(network.data(),"setMsgInspectPeriodMsec", 
                                  Qt::QueuedConnection, Q_ARG(int, msgInspectMsec));
    emit msgInspectMsecChanged(msgInspectMsec);
}


/***************** SCANNER ****************/
/***** Scanner: Service ****/
/* control */
void ScannerFacade::connectToScanner()
{
    /* dethach old scanner */
    if(!scanner.isNull()){
        disconnectFromScanner();
        while(scanner->scanner()->status() != ScannerStateEnum::DETHACHED)
            QObject::thread()->yieldCurrentThread();
    }
    /* re-create procedure */
    if(scannerRecreReq || scanner.isNull())
        if(!scannerCreareProcedure())
            return;
    /* re-connect procedure */
    if(scannerReconReq)
        if(!scannerConnectProcedure())
            return;
    /* call start() */
    QMetaObject::invokeMethod(scanner.data(), "start", Qt::QueuedConnection,
                              Q_ARG(QString, m_scannerAddr));
}
void ScannerFacade::disconnectFromScanner()
{
    if(!scanner.isNull())
        QMetaObject::invokeMethod(scanner.data(), "stop", Qt::QueuedConnection);
}
/* facade info */
bool ScannerFacade::isScannerReconReq() const{
    return scannerReconReq;
}
/* private */
QString ScannerFacade::scannerAddrValid() const{
    return scanner->scanner()->isAddrValid(m_scannerAddr);
}
void ScannerFacade::setScnrRecreatReq(bool recreReq){
    if(recreReq)
        setScnrReconReq(true);
    if (scannerRecreReq == recreReq)
        return;
    scannerRecreReq = recreReq;
}
void ScannerFacade::setScnrReconReq(bool reconReq){
    if (scannerReconReq == reconReq)
        return;
    scannerReconReq = reconReq;
    emit scannerReconReqChanged(scannerReconReq);
}
bool ScannerFacade::scannerCreareProcedure()
{
    /* create new scanner */
    Scanner * device = nullptr;
    switch (m_scannerType) {
    case ScannerFacade::EMULATOR:
        device = new ScannerEmulator();
        break;
    case ScannerFacade::ADS_USB:
        break;
    case ScannerFacade::LINK_SPRITE:
        break;
    }
    /* create new scanner nanenger */
    scanner.reset(new ScannerManengerBasicV1(device));
    /* move to scanner manenger thread */
    scanner->moveToThread(&this->scannerManengerThread);
    /* connections */
    connect(scanner.data(), &ScannerManenger::fieldChanged,
            this, &ScannerFacade::fieldChangedHandler);
    connect(scanner->scanner(), &Scanner::statusChanged,
            this, &ScannerFacade::scannerStateChanged);
    /* reset flags */
    setScnrRecreatReq(false);
    setScnrReconReq(true);
    return true;
}
bool ScannerFacade::scannerConnectProcedure()
{
    /* at this moment here is nothing */
    
    setScnrReconReq(false);
    return true;
}
void ScannerFacade::setField(QVariantList field)
{
    if (m_field == field)
        return;
    
    m_field = field;
    emit fieldChanged(m_field);
}
void ScannerFacade::fieldChangedHandler(ScannerManengerTagField::TagFieldList field)
{
    QVariantList tmp;
    tmp.reserve(field.size());
    foreach (auto &tagStat, field) 
        tmp.append(QVariant::fromValue(tagStat.data()));
    //m_field.clear();
    setField(tmp);
}
/**** Network: Settings ****/
/* internal info */
ScannerFacade::ScannerStateEnum ScannerFacade::scannerState() const{
    return scanner->scanner()->status();
}
QVariantList ScannerFacade::field() const{
    return m_field;
}
/* re-creation settings */
ScannerFacade::ScannerType ScannerFacade::scannerType() const{
    return m_scannerType;
}
QString ScannerFacade::scannerAddr() const{
    return m_scannerAddr;
}
void ScannerFacade::setScannerType(ScannerFacade::ScannerType scannerType){
    if (m_scannerType == scannerType)
        return;
    m_scannerType = scannerType;
    setScnrRecreatReq(true);
    emit scannerTypeChanged(m_scannerType);
}
void ScannerFacade::setScannerAddr(QString scannerAddr){
    if (m_scannerAddr == scannerAddr)
        return;
    m_scannerAddr = scannerAddr;
    setScnrReconReq(true);
    emit scannerAddrValidChanged(scannerAddrValid());
    emit scannerAddrChanged(m_scannerAddr);
}
/* direct settings */
uint ScannerFacade::scanPeriodMsec() const{
    return scanner->timings().scanPerionMsec;
}
uint ScannerFacade::maxUnreadMsec() const{
    return scanner->timings().maxUnreadToLeftMsec;
}
uint ScannerFacade::maxUnreadPcnt() const{
    return scanner->timings().maxUnreadToLeftPcnt;
}
uint ScannerFacade::unreadToDelSec() const{
    return scanner->timings().maxUnreadToDeleteSec;
}
ScannerFacade::TagFieldLeftRuleEnum ScannerFacade::tagLeftRule() const{
    return scanner->timings().leftRule;
}
void ScannerFacade::setScanPeriodMsec(uint scanPeriodMsec)
{
    if (scanner->timings().scanPerionMsec == scanPeriodMsec)
        return;
    
    scanner->timings().scanPerionMsec = scanPeriodMsec;
    emit scanPeriodMsecChanged(scanner->timings().scanPerionMsec);
}
void ScannerFacade::setMaxPeriodMsec(uint maxUnreadMsec)
{
    if (scanner->timings().maxUnreadToLeftMsec == maxUnreadMsec)
        return;
    
    scanner->timings().maxUnreadToLeftMsec = maxUnreadMsec;
    emit maxUnreadMsecChanged(scanner->timings().maxUnreadToLeftMsec);
}
void ScannerFacade::setMaxUnreadPcnt(uint maxUnreadPcnt)
{
    if (scanner->timings().maxUnreadToLeftPcnt == maxUnreadPcnt)
        return;
    
    scanner->timings().maxUnreadToLeftPcnt = maxUnreadPcnt;
    emit maxUnreadPcntChanged(scanner->timings().maxUnreadToLeftPcnt);
}
void ScannerFacade::setUnreadToDelSec(uint unreadToDelSec)
{
    if (scanner->timings().maxUnreadToDeleteSec == unreadToDelSec)
        return;
    
    scanner->timings().maxUnreadToDeleteSec = unreadToDelSec;
    emit unreadToDelSecChanged(scanner->timings().maxUnreadToDeleteSec);
}
void ScannerFacade::setTagLeftRule(ScannerFacade::TagFieldLeftRuleEnum tagLeftRule)
{
    if (scanner->timings().leftRule == tagLeftRule)
        return;
    
    scanner->timings().leftRule = tagLeftRule;
    emit tagLeftRuleChanged(scanner->timings().leftRule);
}
/*** service ***/
QVariantMap ScannerFacade::getEnumFields(QString enuemeration){
    int indx = metaObject()->indexOfEnumerator(enuemeration.toStdString().c_str());
    if(indx < 0)
        return QVariantMap();
    auto me = metaObject()->enumerator(indx);
    QVariantMap map;
    for(int i = 0; i < me.keyCount(); i++)
        map[QString(me.valueToKey(i))] = me.value(i);
    return map;
}


/***************** SYSTEM *****************/




