#include "scannerfacade.h"

ScannerFacade::ScannerFacade()
{

}

bool ScannerFacade::isReady() const
{
    return m_isReady;
}

ScannerFacade::Socket ScannerFacade::socket() const
{
    return m_socket;
}

QString ScannerFacade::server() const
{
    return m_server;
}

quint16 ScannerFacade::port() const
{
    return m_port;
}

ScannerFacade::MsgBound ScannerFacade::msgBoundaries() const
{
    return m_msgBoundaries;
}

QString ScannerFacade::startSqns() const
{
    return m_startSqns;
}

QString ScannerFacade::tailSqns() const
{
    return m_tailSqns;
}

QString ScannerFacade::username() const
{
    return m_username;
}

QString ScannerFacade::password() const
{
    return m_password;
}

ScannerFacade::State ScannerFacade::state() const
{
    return m_state;
}

QString ScannerFacade::stateMsg() const
{
    return m_stateMsg;
}

ScannerFacade::Mode ScannerFacade::mode() const
{
    return m_mode;
}

uint ScannerFacade::msgTxRepeatSec() const
{
    return m_msgTxRepeatSec;
}

uint ScannerFacade::msgMaxTxAtempt() const
{
    return m_msgMaxTxAtempt;
}

qint32 ScannerFacade::msgInspectMsec() const
{
    return m_msgInspectMsec;
}

QString ScannerFacade::logfile() const
{
    return m_logfile;
}

void ScannerFacade::setServer(QString server)
{
    if (m_server == server)
        return;

    m_server = server;
    emit serverChanged(m_server);
}

void ScannerFacade::setPort(quint16 port)
{
    if (m_port == port)
        return;

    m_port = port;
    emit portChanged(m_port);
}

void ScannerFacade::setStartSqns(QString startSqns)
{
    if (m_startSqns == startSqns)
        return;

    m_startSqns = startSqns;
    emit startSqnsChanged(m_startSqns);
}

void ScannerFacade::setTailSqns(QString tailSqns)
{
    if (m_tailSqns == tailSqns)
        return;

    m_tailSqns = tailSqns;
    emit tailSqnsChanged(m_tailSqns);
}

void ScannerFacade::setUsername(QString username)
{
    if (m_username == username)
        return;

    m_username = username;
    emit usernameChanged(m_username);
}

void ScannerFacade::setPassword(QString password)
{
    if (m_password == password)
        return;

    m_password = password;
    emit passwordChanged(m_password);
}

void ScannerFacade::setMode(ScannerFacade::Mode mode)
{
    if (m_mode == mode)
        return;

    m_mode = mode;
    emit modeChanged(m_mode);
}

void ScannerFacade::setMsgTxRepeatSec(uint msgTxRepeatSec)
{
    if (m_msgTxRepeatSec == msgTxRepeatSec)
        return;

    m_msgTxRepeatSec = msgTxRepeatSec;
    emit msgTxRepeatSecChanged(m_msgTxRepeatSec);
}

void ScannerFacade::setMsgMaxTxAtempt(uint msgMaxTxAtempt)
{
    if (m_msgMaxTxAtempt == msgMaxTxAtempt)
        return;

    m_msgMaxTxAtempt = msgMaxTxAtempt;
    emit msgMaxTxAtemptChanged(m_msgMaxTxAtempt);
}

void ScannerFacade::setMsgInspectMsec(qint32 msgInspectMsec)
{
    if (m_msgInspectMsec == msgInspectMsec)
        return;

    m_msgInspectMsec = msgInspectMsec;
    emit msgInspectMsecChanged(m_msgInspectMsec);
}

void ScannerFacade::setLogfile(QString logfile)
{
    if (m_logfile == logfile)
        return;

    m_logfile = logfile;
    emit logfileChanged(m_logfile);
}
