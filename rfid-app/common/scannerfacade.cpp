#include "scannerfacade.h"

ScannerFacade::ScannerFacade(QObject *parent) : Eventful(parent),
    logger(new Logger)
{
    netReCreateRequired = true;
    netReConectRequired = true;

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


}

void ScannerFacade::disconnectFromServer()
{

}

ScannerFacade::NetStatus ScannerFacade::isReady() {
    uint8_t stat = OK;
    if(m_server.isEmpty())
        stat |= NO_SERV;
    if(m_port == 0)
        stat |= NO_PORT;
    if(m_username.isEmpty())
        stat |= NO_USER;
    if(m_password.isEmpty())
        stat |= NO_PASS;
    putStatusToLog((NetStatus)stat);
    emit isReadyChanged((NetStatus)stat);
    return (NetStatus)stat;
}
ScannerFacade::NetState ScannerFacade::netState() const{
    if(network.isNull())
        return NetState::DISCONNECTED;
    return network->getState().getState();
}
QString ScannerFacade::netStateMsg() const{
    if(network.isNull())
        return QStringLiteral("Network object didn't create");
    return network->getState().getMsg();
}
void ScannerFacade::putStatusToLog(ScannerFacade::NetStatus isReady){
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
void ScannerFacade::netStateChanged(NetCommanderState state){
    emit netStateChanged(state.getState());
    emit netStateMsgChanged(state.getMsg());
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
    netReCreateRequired = true;
    m_socket = socket;
    emit socketChanged(m_socket);
}
void ScannerFacade::setMsgBoundaries(ScannerFacade::MsgBound msgBoundaries){
    if (m_msgBoundaries == msgBoundaries)
        return;
    netReCreateRequired = true;
    m_msgBoundaries = msgBoundaries;
    emit msgBoundariesChanged(m_msgBoundaries);
}
void ScannerFacade::setStartSqns(QString startSqns){
    if (m_startSqns == startSqns)
        return;
    netReCreateRequired = true;
    m_startSqns = startSqns;
    emit startSqnsChanged(m_startSqns);
}
void ScannerFacade::setTailSqns(QString tailSqns){
    if (m_tailSqns == tailSqns)
        return;
    netReCreateRequired = true;
    m_tailSqns = tailSqns;
    emit tailSqnsChanged(m_tailSqns);
}

void ScannerFacade::setServer(QString server){
    if (m_server == server)
        return;
    netReConectRequired = true;
    m_server = server;
    emit serverChanged(m_server);
}
void ScannerFacade::setPort(quint16 port){
    if (m_port == port)
        return;
    netReConectRequired = true;
    m_port = port;
    emit portChanged(m_port);
}
void ScannerFacade::setUsername(QString username){
    if (m_username == username)
        return;
    netReConectRequired = true;
    m_username = username;
    emit usernameChanged(m_username);
}
void ScannerFacade::setPassword(QString password){
    if (m_password == password)
        return;
    netReConectRequired = true;
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
