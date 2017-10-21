#include "d_client.h"


/********************** Level 4(D) ***********************/

/*************** Interface ****************/

/****** NetClientState *****/
void NetClientState::fromPhyState(const NetPhyState &state){
    QMutexLocker(&this->access);
    this->state = fromPhyStateHelper(state.getState());
    msg = state.toString();
}
void NetClientState::fromRawState(NetClientState::NetClientStateEnum state, QString msg){
    QMutexLocker(&this->access);
    this->state = state;
    this->msg = msg;
}
NetClientState::NetClientStateEnum NetClientState::stateEnum() const{
    QMutexLocker(&this->access);
    return state;
}
QString NetClientState::stateMessage() const{
    QMutexLocker(&this->access);
    return msg;
}
bool NetClientState::operator ==(NetClientState::NetClientStateEnum state) const{
    QMutexLocker(&this->access);
    return this->state == state;
}
NetClientState::NetClientStateEnum NetClientState::fromPhyStateHelper(QAbstractSocket::SocketState state){
    switch (state) {
    case QAbstractSocket::HostLookupState: return NetClientStateEnum::LOOKUPING;
    case QAbstractSocket::ConnectingState: return NetClientStateEnum::CONNECTING;
    case QAbstractSocket::ConnectedState: return NetClientStateEnum::CONNECTED;
    case QAbstractSocket::ClosingState: return NetClientStateEnum::CLOSING;
    case QAbstractSocket::UnconnectedState:
    case QAbstractSocket::BoundState:
    case QAbstractSocket::ListeningState:
    default:
        return NetClientStateEnum::DISCONNECTED;
    }
}

/******** NetClient ********/
NetClient::NetClient(NetPhy* transport,
                           NetProtocol* protocol,
                           QObject *parent) :
    Eventful (parent), phy(transport), proto(protocol)
{
    qRegisterMetaType<NetClientState*>();
    qRegisterMetaType<QAuthenticator>();
    qRegisterMetaType<NetClientModeEnum>();

    proto->setParent(this);
    phy->setParent(nullptr);
    phy->moveToThread(&phyThread);

    proto->connectAsEventDrain(this);
    phy->connectAsEventDrain(this);

    connect(this, SIGNAL(send(QByteArray)),
            phy.data(), SLOT(send(QByteArray)),
            Qt::QueuedConnection);
    connect(phy.data(), SIGNAL(recv(QByteArray)),
            this,SLOT(recv(QByteArray)),
            Qt::QueuedConnection);
    connect(phy.data(), SIGNAL(stateChanged(NetPhyState)),
            this,SLOT(NetPhyStateHandler(NetPhyState)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(connectToHost(NetPoint)),
            phy.data(),SLOT(connectToHost(NetPoint)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(disconnectFromHost()),
            phy.data(), SLOT(disconnectFromHost()),
            Qt::QueuedConnection);

    phyThread.start();
}
NetClient::~NetClient(){
    phyThread.quit();
    phyThread.wait();
}
const NetClientState *NetClient::state() const{
    return &m_state;
}
void NetClient::NetPhyStateHandler(NetPhyState netPhyState){
    this->m_state.fromPhyState(netPhyState);
    if(this->m_state == NetClientStateClass::AUTHENTICATED)
        emit sysEvent(QSharedPointer<Event> (
                          new NetworkEvent(NetworkEvent::INFO,
                                           NetworkEvent::IDs::AUTHENTICATOR,
                                           m_state.stateMessage())));
    emit stateChanged(&this->m_state);
}
void NetClient::recv(QByteArray data){
    NetProtocol::NetProtocolParseErr err;
    auto msg = proto->parse(data,&err);
    if(err == NetProtocol::PARSE_ERR_OK)
        receiveMsg(msg);
}
void NetClient::transmitMsg(QByteArray msg){
    emit send(proto->pack(msg));
}


/************ Implementation **************/

/***** NetClientV1Basic ****/
NetClientV1Basic::NetClientV1Basic(NetPhy *transport,
                             NetProtocol *protocol,
                             QObject *parent ):
    NetClient(transport,protocol,parent),
    mode(NetClientModeEnum::DISABLED), inspectTimer(this),
    msgTransmitRepeatSec(MSG_TRANSMIT_REPEAT_SEC),
    msgMaxAtemptToDelete(MSG_TRANSMIT_DELETE_NUM)
{
    connect(this, SIGNAL(stateChanged(const NetClientState*)),
            this, SLOT(netClientStateChangedHelperHandler(const NetClientState*)));
    connect(&inspectTimer, SIGNAL(timeout()), this,SLOT(msgInspect()));
    inspectTimer.setInterval(MSG_INSPECT_PERIOD_MSEC);
}

/* control */
void NetClientV1Basic::start(){
    if(addr.isNull()){
        emit sysEvent(QSharedPointer<Event> (
                          new NetworkEvent(NetworkEvent::WARNING,
                                           NetworkEvent::IDs::COMMANDER,
                                           QStringLiteral("Try ty connect, but "
                                                          "address and port "
                                                          "didn't set."))));
        return;
    }
    emit connectToHost(addr);
    inspectTimer.start();
}
void NetClientV1Basic::stop(){
    inspectTimer.stop();
    emit disconnectFromHost();
}

/* settings */
QAuthenticator NetClientV1Basic::getAuth() const{
    return auth;
}
void NetClientV1Basic::setAuth(const QAuthenticator &value){
    auth = value;
}
NetClientV1Basic::NetClientModeEnum NetClientV1Basic::getMode() const{
    return mode;
}
void NetClientV1Basic::setMode(NetClientModeEnum value){
    mode = value;
}
NetPoint NetClientV1Basic::getAddr() const{
    return addr;
}
void NetClientV1Basic::setAddr(const NetPoint &value){
    addr = value;
}

/* todo routine */
void NetClientV1Basic::receiveMsg(QByteArray data){
    //todo
}
void NetClientV1Basic::netClientStateChangedHelperHandler(const NetClientState *state){
    //todo
}

/* msg send routine */
void NetClientV1Basic::netEventIn(QSharedPointer<Event> event){
    sendMsgEnqueue(QSharedPointer<NetMessage>(event->event == Event::TAG ?
                                                  static_cast<NetMessage*>(new TagEventMsg(event->toJson())) :
                                                  static_cast<NetMessage*>(new ErrEventMsg(event->toJson()))));
}
void NetClientV1Basic::sendMsgEnqueue(QSharedPointer<NetMessage> msg){
    messageQueue.enqueue(msg);
    if(mode == NetClientModeEnum::EVENT)
        sendMsgDirect(msg);
}
void NetClientV1Basic::sendMsgDirect(QSharedPointer<NetMessage> msg){
    if(mode == NetClientModeEnum::DISABLED)
        return;
    emit transmitMsg(msg->pack(auth));
}

/* msg repeated transmission routine */
void NetClientV1Basic::msgInspect()
{
    foreach (auto msg, messageQueue) {
        if(msg->getTransmitCount() > 0){
            if(msg->getTransmitCount() >= msgMaxAtemptToDelete){
                emit sysEvent(QSharedPointer<Event> (
                                  new NetworkEvent(NetworkEvent::INFO,
                                                   NetworkEvent::IDs::COMMANDER,
                                                   QStringLiteral("Message ") +
                                                   msg->uuid.toString() +
                                                   QStringLiteral(" lost. It was have ") +
                                                   QString::number(msg->getTransmitCount()) +
                                                   QStringLiteral(" transmission repeat attempt."))));
                messageQueue.removeOne(msg);
            }
            else if(msg->getLastTransmit().secsTo(QDateTime::currentDateTime())
                    >= msgTransmitRepeatSec){
                emit sysEvent(QSharedPointer<Event> (
                                  new NetworkEvent(NetworkEvent::WARNING,
                                                   NetworkEvent::IDs::COMMANDER,
                                                   QStringLiteral("Message ") +
                                                   msg->uuid.toString() +
                                                   QStringLiteral(" transmission repeat #")+
                                                   QString::number(msg->getTransmitCount()))));
                sendMsgDirect(msg);
            }
        }
    }
}
int NetClientV1Basic::getMsgInspectPeriodMsec() const{
    return inspectTimer.interval();
}
void NetClientV1Basic::setMsgInspectPeriodMsec(int value){
    inspectTimer.setInterval(value);
}
uint NetClientV1Basic::getMsgMaxAtemptToDelete() const{
    return msgMaxAtemptToDelete;
}
void NetClientV1Basic::setMsgMaxAtemptToDelete(uint value){
    msgMaxAtemptToDelete = value;
}
uint NetClientV1Basic::getMsgTransmitRepeatSec() const{
    return msgTransmitRepeatSec;
}
void NetClientV1Basic::setMsgTransmitRepeatSec(uint value){
    msgTransmitRepeatSec = value;
}






