#include "scannerfacade.h"

ScannerFacade::ScannerFacade(QObject *parent) : Eventful(parent),
    logger(new Logger)
{
    netReCreateRequire = true;
    netReConectRequire = true;

    m_socket = Socket::TCP;
    m_msgBoundaries = MsgBound::BOUND_V1;
    m_startSqns = QStringLiteral("$SD#");
    m_tailSqns = QStringLiteral("\r\n\r\n");
    //    m_authType = AuthType::JSON;
    m_mode = Mode::DISABLED;
    m_msgTxRepeatSec = MSG_TRANSMIT_REPEAT_SEC;
    m_msgMaxTxAtempt = MSG_TRANSMIT_DELETE_NUM;
    m_msgInspectMsec = MSG_INSPECT_PERIOD_MSEC;

    connect(this, SIGNAL(sysEvent(QSharedPointer<Event>)),
            logger.data(),SLOT(sysEventIn(QSharedPointer<Event>)));

    logManengerThread.start();
    netManengerThread.start();
}

ScannerFacade::~ScannerFacade()
{
    netManengerThread.quit();
    netManengerThread.wait();

    logManengerThread.quit();
    logManengerThread.wait();
}

void ScannerFacade::connectToServer()
{
    if(netReCreateRequire){
        if(!network.isNull()){
            network->stop();
            while(*network->state() != NetClientStateEnum::DISCONNECTED)
                QObject::thread()->yieldCurrentThread();
        }
    }

}

void ScannerFacade::disconnectFromServer()
{

}

/* net: is reconnection required */
bool ScannerFacade::isReconRequire() const{
    return netReCreateRequire || netReConectRequire;
}
void ScannerFacade::setNetReCreateRequire(bool require){
    if(netReCreateRequire != require)
        emit isReconRequireChanged(isReconRequire());
    isReady();
    netReCreateRequire = require;
}
void ScannerFacade::setNetReConectRequire(bool require){
    if(netReConectRequire != require)
        emit isReconRequireChanged(isReconRequire());
    isReady();
    netReConectRequire = require;
}

/* net: is all required fields are fill */
ScannerFacade::NetFillFieldStatus ScannerFacade::isReady() {
    uint8_t stat = OK;
    if(m_server.isEmpty())
        stat |= NO_SERV;
    if(m_port == 0)
        stat |= NO_PORT;
    if(m_username.isEmpty())
        stat |= NO_USER;
    if(m_password.isEmpty())
        stat |= NO_PASS;
    putStatusToLog((NetFillFieldStatus)stat);
    emit isReadyChanged((NetFillFieldStatus)stat);
    return (NetFillFieldStatus)stat;
}
void ScannerFacade::putStatusToLog(ScannerFacade::NetFillFieldStatus isReady){
    if(isReady&NO_SERV)
        emit sysEvent(QSharedPointer<Event> (
                          new SystemEvent(SystemEvent::WARNING,
                                          SystemEvent::IDs::FACADE_STATUS,
                                          QStringLiteral("Server address is not set."))));
    if(isReady&NO_PORT)
        emit sysEvent(QSharedPointer<Event> (
                          new SystemEvent(SystemEvent::WARNING,
                                          SystemEvent::IDs::FACADE_STATUS,
                                          QStringLiteral("Server port is not set.(port==0)"))));
    if(isReady&NO_USER)
        emit sysEvent(QSharedPointer<Event> (
                          new SystemEvent(SystemEvent::WARNING,
                                          SystemEvent::IDs::FACADE_STATUS,
                                          QStringLiteral("Username is not set"))));
    if(isReady&NO_PASS)
        emit sysEvent(QSharedPointer<Event> (
                          new SystemEvent(SystemEvent::WARNING,
                                          SystemEvent::IDs::FACADE_STATUS,
                                          QStringLiteral("Password is not set"))));
}

/* net: info */
void ScannerFacade::netStateChangedHandler(const NetClientState *state){
    emit netStateChanged(state->stateEnum());
    emit netStateMsgChanged(state->stateMessage());
}
ScannerFacade::NetClientStateEnum ScannerFacade::netState() const{
    if(network.isNull())
        return NetClientStateEnum::DISCONNECTED;
    return network->state()->stateEnum();
}
QString ScannerFacade::netStateMsg() const{
    if(network.isNull())
        return QStringLiteral("Network object didn't create");
    return network->state()->stateMessage();
}

ScannerFacade::Socket ScannerFacade::socket() const{
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

ScannerFacade::Mode ScannerFacade::mode() const{
    if(network.isNull())
        return m_mode;
    return network->getMode();
}
uint ScannerFacade::msgTxRepeatSec() const{
    if(network.isNull())
        return m_msgTxRepeatSec;
    return network->getMsgTransmitRepeatSec();
}
uint ScannerFacade::msgMaxTxAtempt() const{
    if(network.isNull())
        return m_msgMaxTxAtempt;
    return network->getMsgMaxAtemptToDelete();
}
qint32 ScannerFacade::msgInspectMsec() const{
    if(network.isNull())
        return m_msgInspectMsec;
    return network->getMsgInspectPeriodMsec();
}

QString ScannerFacade::logfile() const{
    return logger->getLogfilePath();
}


void ScannerFacade::setSocket(ScannerFacade::Socket socket){
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

void ScannerFacade::setServer(QString server){
    if (m_server == server)
        return;
    setNetReConectRequire(true);
    m_server = server;
    emit serverChanged(m_server);
}
void ScannerFacade::setPort(quint16 port){
    if (m_port == port)
        return;
    setNetReConectRequire(true);
    m_port = port;
    emit portChanged(m_port);
}
void ScannerFacade::setUsername(QString username){
    if (m_username == username)
        return;
    setNetReConectRequire(true);
    m_username = username;
    emit usernameChanged(m_username);
}
void ScannerFacade::setPassword(QString password){
    if (m_password == password)
        return;
    setNetReConectRequire(true);
    m_password = password;
    emit passwordChanged(m_password);
}

void ScannerFacade::setMode(ScannerFacade::Mode mode){
    if (m_mode == mode)
        return;
    m_mode = mode;
    emit modeChanged(m_mode);
    if(network.isNull())
        return;
    network->setMode(mode);
}
void ScannerFacade::setMsgTxRepeatSec(uint msgTxRepeatSec){
    if (m_msgTxRepeatSec == msgTxRepeatSec)
        return;
    m_msgTxRepeatSec = msgTxRepeatSec;
    emit msgTxRepeatSecChanged(m_msgTxRepeatSec);
    if(network.isNull())
        return;
    network->setMsgTransmitRepeatSec(msgTxRepeatSec);
}
void ScannerFacade::setMsgMaxTxAtempt(uint msgMaxTxAtempt){
    if (m_msgMaxTxAtempt == msgMaxTxAtempt)
        return;
    m_msgMaxTxAtempt = msgMaxTxAtempt;
    emit msgMaxTxAtemptChanged(m_msgMaxTxAtempt);
    if(network.isNull())
        return;
    network->setMsgMaxAtemptToDelete(msgMaxTxAtempt);
}
void ScannerFacade::setMsgInspectMsec(qint32 msgInspectMsec){
    if (m_msgInspectMsec == msgInspectMsec)
        return;
    m_msgInspectMsec = msgInspectMsec;
    emit msgInspectMsecChanged(m_msgInspectMsec);
    if(network.isNull())
        return;
    network->setMsgInspectPeriodMsec(msgInspectMsec);
}

void ScannerFacade::setLogfile(QString logfile){
    if (this->logfile() == logfile)
        return;
    logger->setLogfilePath(logfile);
    emit logfileChanged(logfile);
}
