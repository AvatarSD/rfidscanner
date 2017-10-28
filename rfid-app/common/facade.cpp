#include "facade.h"
#include <QtQml>


ScannerFacade::ScannerFacade(QObject *parent) : Eventful(parent),
    logManengerThread(this), logger(new Logger),
    netManengerThread(this), scannerManengerThread(this)// sysManengerThread(this);
{    
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
    qmlRegisterType<ScannerFacade>("ScannerMainElements",1,0,"Facade");
    qmlRegisterType<NetClientModeClass>("ScannerMainElements",1,0,"NetMode");
    qmlRegisterType<NetClientStateClass>("ScannerMainElements",1,0,"NetState");
    /* todo: test without this */
    netReCreateRequire = true;
    netReConectRequire = true;
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
    // todo: register qml for rfid manamger
    // quened connections of rfid manamger
    scannerManengerThread.setObjectName("Scanner Manenger");
    scannerManengerThread.start();
    
    
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
        QMetaObject::invokeMethod(network.data(), "stop", Qt::QueuedConnection);
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
ScannerFacade::NetSettStat ScannerFacade::netSettStat() {
    uint8_t stat = OK;
    if(m_server.isEmpty())
        stat |= NO_SERV;
    if(m_port == 0)
        stat |= NO_PORT;
    if(m_username.isEmpty())
        stat |= NO_USER;
    if(m_password.isEmpty())
        stat |= NO_PASS;
    return (NetSettStat)stat;
}
/* private */
void ScannerFacade::netStateChangedHandler(const NetClientState *state){
    emit netStateChanged(state->stateEnum());
    emit netStateMsgChanged(state->stateMessage());
}
void ScannerFacade::chkNetSettStat(ScannerFacade::NetSettStat netSettStat){
    emit netSettStatChanged(netSettStat);
    putNetSettStatToLog(netSettStat);
}
void ScannerFacade::putNetSettStatToLog(ScannerFacade::NetSettStat netSettStat){
    if(netSettStat&NO_SERV)
        emit sysEvent(QSharedPointer<Event> (
                          new SystemEvent(SystemEvent::ERROR,
                                          SystemEvent::IDs::FACADE_STATUS,
                                          QStringLiteral("Server address is not set."))));
    if(netSettStat&NO_PORT)
        emit sysEvent(QSharedPointer<Event> (
                          new SystemEvent(SystemEvent::ERROR,
                                          SystemEvent::IDs::FACADE_STATUS,
                                          QStringLiteral("Server port is 0."))));
    if(netSettStat&NO_USER)
        emit sysEvent(QSharedPointer<Event> (
                          new SystemEvent(SystemEvent::WARNING,
                                          SystemEvent::IDs::FACADE_STATUS,
                                          QStringLiteral("Username is not set. "
                                                         "Connection without login."))));
    if(netSettStat&NO_PASS)
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
    auto ret = netSettStat();
    chkNetSettStat(ret);
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
/* info(internal) */
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
    emit netSettStatChanged(netSettStat());
}
void ScannerFacade::setPort(quint16 port){
    if (m_port == port)
        return;
    m_port = port;
    setNetReConectRequire(true);
    emit portChanged(m_port);
    emit netSettStatChanged(netSettStat());
}
void ScannerFacade::setUsername(QString username){
    if (m_username == username)
        return;
    m_username = username;
    setNetReConectRequire(true);
    emit usernameChanged(m_username);
    emit netSettStatChanged(netSettStat());
}
void ScannerFacade::setPassword(QString password){
    if (m_password == password)
        return;
    m_password = password;
    setNetReConectRequire(true);
    emit passwordChanged(m_password);
    emit netSettStatChanged(netSettStat());
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
void ScannerFacade::connectToScanner()
{
    
}
void ScannerFacade::disconnectFromScanner()
{
    
}


/***************** SYSTEM *****************/




