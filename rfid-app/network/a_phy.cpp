#include "a_phy.h"
#include <QTcpSocket>


/********************** Level 1(A) ***********************/

/*************** Interface ****************/

/********* Supply **********/
#define RECONNECT_TIME 50
#define RECONNECT_ERR_TIME 1500
#define RECONNECT_CRI_TIME 10000
#define RECONNECT_EXT_TIME 100000

/******** NetPoint *********/
NetPoint::NetPoint() : _addr(), _port(0){
    qRegisterMetaType<NetPoint>();
}
NetPoint::NetPoint(QString addr, quint16 port) : _addr(addr), _port(port){
    qRegisterMetaType<NetPoint>();
}
QString NetPoint::addr() const{
    return _addr;
}
void NetPoint::addr(QString addr){
    _addr = addr;
}
quint16 NetPoint::port() const{
    return _port;
}
void NetPoint::port(quint16 port){
    _port = port;
}
bool NetPoint::isNull() const{
    return (_addr.isNull() && (_port == 0));
}
void NetPoint::reset(){
    _addr.clear(); _port = 0;
}

/******** NetState *********/
NetPhyState::NetPhyState(QAbstractSocket::SocketState state, QString message) :
    state(state), msg(std::move(message)){
}
QString NetPhyState::toString() const{
    return msg.isEmpty() ? toRawString() : msg;
}
bool NetPhyState::operator ==(QAbstractSocket::SocketState state){
    return (this->state == state);
}
QString NetPhyState::toRawString() const{
    switch (state) {
    case QAbstractSocket::UnconnectedState: return QStringLiteral("Unconnected");
    case QAbstractSocket::HostLookupState: return QStringLiteral("Host Lookuping");
    case QAbstractSocket::ConnectingState: return QStringLiteral("Connecting");
    case QAbstractSocket::ConnectedState: return QStringLiteral("Connected");
    case QAbstractSocket::BoundState: return QStringLiteral("Bound");
    case QAbstractSocket::ListeningState: return QStringLiteral("Listening");
    case QAbstractSocket::ClosingState: return QStringLiteral("Closing");
    default: return QStringLiteral("Undefined"); }
}
QAbstractSocket::SocketState NetPhyState::getState() const{
    return state;
}


/************ Implementation **************/

/****** NetTransport *******/
NetPhyTcp::NetPhyTcp(QObject* parent) :
    NetPhyTcp(new QTcpSocket(),parent){
    socket->setParent(this);
}
NetPhyTcp::NetPhyTcp(QAbstractSocket *socket, QObject *parent) :
    NetPhy(parent), socket(socket), zerotimer(this)
{
    qRegisterMetaType<QAbstractSocket::SocketState>();
    qRegisterMetaType<QAbstractSocket::SocketError>();
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(socketReadyRead()));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(&zerotimer, SIGNAL(timeout()),
            this, SLOT(run()));
    zerotimer.setTimerType(Qt::CoarseTimer);
    //    zerotimer.setSingleShot(true);
    zerotimer.start(RECONNECT_TIME);
}
NetPhyTcp::~NetPhyTcp(){
    zerotimer.stop();
    socket->disconnectFromHost();
}
NetPhyState NetPhyTcp::state() const{
    auto state = socket->state();
    QString stateMsg = stateToString(state, host);
    if((state == QAbstractSocket::UnconnectedState) && zerotimer.isActive())
            stateMsg += QStringLiteral(" ") + socket->errorString();
    return NetPhyState(state, std::move(stateMsg));
}
QString NetPhyTcp::stateToString(QAbstractSocket::SocketState state,
                                       const NetPoint& host)
{
    QString msg;
    switch(state){
    case QAbstractSocket::UnconnectedState:
        msg = "Socket unconnected.";
        break;
    case QAbstractSocket::HostLookupState:
        msg = "Attempt to connect to: " + host.addr() + ":" +
                QString::number(host.port());
        break;
    case QAbstractSocket::ConnectingState:
        msg = host.addr() + " look-up'ed. Try to connect to " +
                QString::number(host.port()) + " port.";
        break;
    case QAbstractSocket::ConnectedState:
        msg = "Connected to: " + host.addr() + ":" +
                QString::number(host.port());
        break;
    case QAbstractSocket::ClosingState:
        msg = "Socket is about to close";
        break;
    case QAbstractSocket::BoundState:
    case QAbstractSocket::ListeningState:
    default:
        msg = "Socket go to unexpected state. =((";  break;
    }
    return msg;
}
/**** IO operations ****/
qint32 NetPhyTcp::send(QByteArray data){
    if(socket->state() == QAbstractSocket::ConnectedState)
        return socket->write(data);
    else return -1;
}
void NetPhyTcp::socketReadyRead(){
    emit recv(socket->readAll());
}
/**** connection estab. operations ****/
void NetPhyTcp::connectToHost(const NetPoint &addr){
    zerotimer.start(RECONNECT_TIME);
    if(!addr.isNull())
        this->host = addr;
    socket->connectToHost(this->host.addr(), this->host.port());
}
void NetPhyTcp::disconnectFromHost(){
    zerotimer.stop();
    socket->disconnectFromHost();
}
/* keepalive functionality */
void NetPhyTcp::run()
{
    if(socket->state() == QAbstractSocket::UnconnectedState)
        connectToHost();
    zerotimer.start(RECONNECT_TIME);
}
void NetPhyTcp::socketStateChanged(QAbstractSocket::SocketState state)
{
    QString eventMsg, stateMsg;
    eventMsg = stateMsg = stateToString(state, host);

    switch (state) {
    case QAbstractSocket::UnconnectedState:
        eventMsg += " Last address was: " +
                host.addr() + ":" + QString::number(host.port());
        if(zerotimer.isActive())
            stateMsg += QStringLiteral(" ") + socket->errorString();
        break;
    case QAbstractSocket::BoundState:
    case QAbstractSocket::ListeningState:
        socket->disconnectFromHost(); break;
    default: break;
    }

    emit sysEvent(QSharedPointer<Event> (
                      new NetworkEvent(NetworkEvent::INFO,
                                       NetworkEvent::IDs::SOCKET_STATE,
                                       std::move(eventMsg))));
    emit stateChanged(NetPhyState(state, std::move(stateMsg)));
}
void NetPhyTcp::socketError(QAbstractSocket::SocketError error)
{
    uint intv = RECONNECT_TIME;
    NetworkEvent::EventLevel lvl = NetworkEvent::ERROR;
    switch (error)
    {
    case QAbstractSocket::DatagramTooLargeError:
    case QAbstractSocket::UnfinishedSocketOperationError:
    case QAbstractSocket::OperationError:
    case QAbstractSocket::TemporaryError:
        /* nor a error*/
        lvl = NetworkEvent::WARNING;
        break;
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::SocketTimeoutError:
    case QAbstractSocket::NetworkError:
    case QAbstractSocket::ProxyConnectionClosedError: // noe
    case QAbstractSocket::ProxyConnectionTimeoutError:  // noe
        /* not critical error -> do reconnect */
        break;
    case QAbstractSocket::ConnectionRefusedError:  // 1000ms   Ex
    case QAbstractSocket::SocketResourceError:     // noe 1000ms
    case QAbstractSocket::AddressInUseError:  // noe
    case QAbstractSocket::SocketAddressNotAvailableError: // noe
    case QAbstractSocket::SslHandshakeFailedError:  // 1000ms   Ex
    case QAbstractSocket::UnknownSocketError:  // noe
    case QAbstractSocket::UnsupportedSocketOperationError:
    case QAbstractSocket::ProxyConnectionRefusedError: // noe
        /* critical error -> reconnnect after wait time*/
        intv = RECONNECT_ERR_TIME;
        break;
    case QAbstractSocket::HostNotFoundError:       // 10000ms
    case QAbstractSocket::SslInvalidUserDataError:
    case QAbstractSocket::SslInternalError:     // 100000
    case QAbstractSocket::SocketAccessError:
    case QAbstractSocket::ProxyNotFoundError:    // 10000ms
    case QAbstractSocket::ProxyProtocolError:   // 10000
        /* critical error -> do not reconnect */
        intv = RECONNECT_CRI_TIME;
        lvl = NetworkEvent::CRITICAL;
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        /*for future*/
        intv = RECONNECT_EXT_TIME;
        break;
    }
    emit sysEvent(QSharedPointer<Event> (
                      new NetworkEvent(
                          lvl,NetworkEvent::SOCKET_ERROR,
                          QStringLiteral("Code ") + QString::number(error) + ": " +
                          socket->errorString() +
                          (intv==RECONNECT_TIME ? "." : ". Sleep for " +
                                                  QString::number(intv) + "ms."))));
    zerotimer.start(intv);
}

