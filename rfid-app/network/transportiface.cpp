#include "transportiface.h"
#include <QTcpSocket>


/************ Supply *************/

/******* NetPoint ********/
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

/******* NetState ********/
NetState::NetState(QAbstractSocket::SocketState state) : _state(state){
}
QString NetState::toString(){
    switch (_state) {
    case QAbstractSocket::UnconnectedState: return QStringLiteral("Unconnected");
    case QAbstractSocket::HostLookupState: return QStringLiteral("HostLookup");
    case QAbstractSocket::ConnectingState: return QStringLiteral("Connecting");
    case QAbstractSocket::ConnectedState: return QStringLiteral("Connected");
    case QAbstractSocket::BoundState: return QStringLiteral("Bound");
    case QAbstractSocket::ListeningState: return QStringLiteral("Listening");
    case QAbstractSocket::ClosingState: return QStringLiteral("Closing");
    default: return QStringLiteral("Undefined"); }
}
NetState &NetState::operator =(QAbstractSocket::SocketState state){
    this->_state = state;
    return *this;
}



/******************** Implementation *********************/

/***** NetTransport ******/
/**** routine ****/
TcpNetTransport::TcpNetTransport(QObject* parent) :
    TcpNetTransport(new QTcpSocket(),parent){
    socket->setParent(this);
}
TcpNetTransport::TcpNetTransport(QAbstractSocket *socket, QObject *parent) :
    NetTransport(parent), socket(socket), zerotimer(this)
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
    zerotimer.start(0);
}
TcpNetTransport::~TcpNetTransport(){
    zerotimer.stop();
}
const QAbstractSocket *TcpNetTransport::getSocket() const{
    return socket.data();
}
NetState TcpNetTransport::currentState() const{
    return socket->state();
}


/**** IO operations ****/
qint32 TcpNetTransport::send(QByteArray data){
    if(socket->state() == QAbstractSocket::ConnectedState)
        return socket->write(data);
    else return -1;
}
void TcpNetTransport::socketReadyRead(){
    emit recv(socket->readAll());
}


/**** connection estab. operations ****/
void TcpNetTransport::connectToHost(NetPoint addr){
    reconnectRequired = true;
    if(!addr.isNull())
        this->host = addr;
    socket->connectToHost(this->host.addr(), this->host.port());
}
void TcpNetTransport::disconnectFromHost(){
    reconnectRequired = false;
    socket->disconnectFromHost();
}


/**** keepalive functionality ****/
void TcpNetTransport::run()
{
    if((socket->state() == QAbstractSocket::UnconnectedState) && reconnectRequired)
        connectToHost();
}
void TcpNetTransport::socketStateChanged(QAbstractSocket::SocketState state)
{
    QString msg;
    NetworkEvent::EventLevel lvl = NetworkEvent::DEBUG;
    switch(state){
    case QAbstractSocket::UnconnectedState:
        msg = "Socket unconnected. Last address was: " +
             host.addr() + ":" + QString::number(host.port());
        lvl = NetworkEvent::WARNING;
        break;
    case QAbstractSocket::HostLookupState:
        msg = "Attempt to connect to: " + host.addr() + ":" +
                QString::number(host.port());
        lvl = NetworkEvent::INFO;
        break;
    case QAbstractSocket::ConnectingState:
        msg = host.addr() + " look-up'ed. Try to connect to " +
                QString::number(host.port()) + " port.";
        lvl = NetworkEvent::INFO;
        break;
    case QAbstractSocket::ConnectedState:
        msg = "Connected to: " + host.addr() + ":" +
                QString::number(host.port());
        lvl = NetworkEvent::INFO;
        break;
    case QAbstractSocket::ClosingState:
        msg = "Socket is about to close";
        lvl = NetworkEvent::INFO;
        break;
    case QAbstractSocket::BoundState:
    case QAbstractSocket::ListeningState:
    default:
        msg = "Socket go to unexpected state. =((";
        lvl = NetworkEvent::DEBUG;
        break;
    }


    emit sysEvent(QSharedPointer<Event> (
                      new NetworkEvent(lvl,
                          NetworkEvent::IDs::SOCKET_STATE, msg)));
    emit stateChanged(state);
}
void TcpNetTransport::socketError(QAbstractSocket::SocketError error)
{
    emit sysEvent(QSharedPointer<Event> (
                      new NetworkEvent(
                          NetworkEvent::WARNING,
                          NetworkEvent::SOCKET_ERROR,
                          QStringLiteral("Code ") + QString::number(error) + ": "
                          + socket->errorString())));
/* todo:
 * - list of timings
 * - timer control
 * - msg swap level and place
 * + thread test
 * + error string conversion
 * + switch state changed events
 * + invoke method
 */

    switch (error)
    {
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::SocketTimeoutError:
    case QAbstractSocket::NetworkError:
    case QAbstractSocket::ProxyConnectionClosedError:
    case QAbstractSocket::ProxyConnectionTimeoutError:
        /* not critical error -> do reconnect */


        break;

    case QAbstractSocket::ConnectionRefusedError:
    case QAbstractSocket::HostNotFoundError:
    case QAbstractSocket::SocketResourceError:
    case QAbstractSocket::AddressInUseError:
    case QAbstractSocket::SocketAddressNotAvailableError:
    case QAbstractSocket::SslHandshakeFailedError:
    case QAbstractSocket::ProxyConnectionRefusedError:
    case QAbstractSocket::ProxyNotFoundError:
    case QAbstractSocket::ProxyProtocolError:
    case QAbstractSocket::SslInternalError:
    case QAbstractSocket::UnknownSocketError:
        /* critical error -> reconnnect after wait time*/


        break;

    case QAbstractSocket::SocketAccessError:
    case QAbstractSocket::UnsupportedSocketOperationError:
    case QAbstractSocket::ProxyAuthenticationRequiredError:
    case QAbstractSocket::SslInvalidUserDataError:
        /* critical error -> do not reconnect */
        break;

    case QAbstractSocket::DatagramTooLargeError:
    case QAbstractSocket::UnfinishedSocketOperationError:
    case QAbstractSocket::OperationError:
    case QAbstractSocket::TemporaryError:
        /* nor a error*/
        break;
    }


    //    setState(NetTransportState::NET_CONN_BROKEN);

}



