#include "networkclient.h"

#include <iostream>


/********************** Interface ************************/

/**** NetCommanderState ****/
NetCommanderState::NetCommanderState(const NetState &state){
    fromNetState(state);
}
void NetCommanderState::fromNetState(const NetState &state){
    this->state = fromSocketState(state.getState());
    msg = state.toString();
}
void NetCommanderState::fromRawState(NetCommanderState::States state, QString msg){
    this->state = state;
    this->msg = msg;
}
void NetCommanderState::fromSelfState(const NetCommanderState &state){
    this->state = state.getState();
    this->msg = state.getMsg();
}
NetCommanderState::States NetCommanderState::getState() const{
    return state;
}
QString NetCommanderState::getMsg() const{
    return msg;
}
bool NetCommanderState::operator ==(NetCommanderState::States state){
    return this->state == state;
}
NetCommanderState::States NetCommanderState::fromSocketState(QAbstractSocket::SocketState state){
    switch (state) {
    case QAbstractSocket::HostLookupState: return States::LOOKUPING;
    case QAbstractSocket::ConnectingState: return States::CONNECTING;
    case QAbstractSocket::ConnectedState: return States::CONNECTED;
    case QAbstractSocket::ClosingState: return States::CLOSING;
    case QAbstractSocket::UnconnectedState:
    case QAbstractSocket::BoundState:
    case QAbstractSocket::ListeningState:
    default:
        return States::DISCONNECTED;
    }
}

/****** NetCommander *******/
NetCommander::NetCommander(NetTransport* transport,
                           NetProtocol* protocol,
                           QObject *parent) :
    Eventful (parent), phy(transport), proto(protocol)
{
    qRegisterMetaType<NetCommanderState>();

    proto->setParent(this);
    phy->setParent(nullptr);
    phy->moveToThread(&phyThread);

    proto->connectAsEventDrain(this);
    phy->connectAsEventDrain(this);

    connect(this, SIGNAL(send(QByteArray)),
            phy.data(), SLOT(send(QByteArray)));
    connect(phy.data(), SIGNAL(recv(QByteArray)),
            this,SLOT(recv(QByteArray)));
    connect(phy.data(), SIGNAL(stateChanged(NetState)),
            this,SLOT(transportStateChanged(NetState)));
    connect(this, SIGNAL(connectToHost(NetPoint)),
            phy.data(),SLOT(connectToHost(NetPoint)));
    connect(this, SIGNAL(disconnectFromHost()),
            phy.data(), SLOT(disconnectFromHost()));

    phyThread.start();
}
NetCommander::~NetCommander(){
    phyThread.quit();
    phyThread.wait();
}
const NetCommanderState &NetCommander::getState() const{
    return state;
}
void NetCommander::transportStateChanged(NetState newState){
    this->setState(NetCommanderState(newState));
}
void NetCommander::setState(const NetCommanderState &state){
    this->state = state;
    if(this->state == NetCommanderState::AUTHENTICATED)
        emit sysEvent(QSharedPointer<Event> (
                          new NetworkEvent(NetworkEvent::INFO,
                                           NetworkEvent::IDs::AUTHENTICATOR,
                                           state.getMsg())));
    emit stateChanged(this->state);
}
void NetCommander::recv(QByteArray data){
    NetProtocol::NetProtocolParseErr err;
    auto msg = proto->parse(data,&err);
    if(err == NetProtocol::PARSE_ERR_OK)
        receiveMsg(msg);
}
void NetCommander::transmitMsg(QByteArray msg){
    emit send(proto->pack(msg));
}


/******************** Implementation *********************/


/****** BasicV1Client ******/
BasicV1Client::BasicV1Client(NetTransport *transport,
                             NetProtocol *protocol,
                             const NetPoint & addr,
                             QObject *parent ):
    NetCommander(transport,protocol,parent),
    addr(addr),mode(POOL), inspectTimer(this),
    msgTransmitRepeatSec(MSG_TRANSMIT_REPEAT_SEC),
    msgMaxAtemptToDelete(MSG_TRANSMIT_DELETE_NUM)
{
    connect(this, SIGNAL(stateChanged(NetCommanderState)),
            this, SLOT(stateChanged(NetCommanderState)));
    connect(&inspectTimer, SIGNAL(timeout()), this,SLOT(msgInspect()));
    inspectTimer.setInterval(MSG_INSPECT_PERIOD_MSEC);
}
void BasicV1Client::start(){
    emit connectToHost(addr);
    QMetaObject::invokeMethod(&inspectTimer, "start", Qt::QueuedConnection);
}
void BasicV1Client::stop(){
    QMetaObject::invokeMethod(&inspectTimer, "stop", Qt::QueuedConnection);
    emit disconnectFromHost();
}
QAuthenticator BasicV1Client::getAuth() const{
    return auth;
}
void BasicV1Client::setAuth(const QAuthenticator &value){
    auth = value;
}
BasicV1Client::WorkMode BasicV1Client::getMode() const{
    return mode;
}
void BasicV1Client::setMode(const WorkMode &value){
    mode = value;
}
NetPoint BasicV1Client::getAddr() const{
    return addr;
}
void BasicV1Client::setAddr(const NetPoint &value){
    addr = value;
}


void BasicV1Client::netEventIn(QSharedPointer<Event> event){
    sendMsgEnqueue(QSharedPointer<NetMessage>(event->event == Event::TAG ?
                                                  static_cast<NetMessage*>(new TagEventMsg(event->toJson())) :
                                                  static_cast<NetMessage*>(new ErrEventMsg(event->toJson()))));
}
void BasicV1Client::receiveMsg(QByteArray data){

}

void BasicV1Client::stateChanged(NetCommanderState state){

}
void BasicV1Client::sendMsgEnqueue(QSharedPointer<NetMessage> msg)
{
    messageQueue.enqueue(msg);
    if(mode == EVENT)
        sendMsgDirect(msg);
}
void BasicV1Client::sendMsgDirect(QSharedPointer<NetMessage> msg)
{
    emit transmitMsg(msg->pack(auth));
}

void BasicV1Client::msgInspect()
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
                                                   QStringLiteral("transmission repeat attempt."))));
                messageQueue.removeOne(msg);
            }
            else if(msg->getLastTransmit().secsTo(QDateTime::currentDateTime())
                    >= msgTransmitRepeatSec){
                emit sysEvent(QSharedPointer<Event> (
                                  new NetworkEvent(NetworkEvent::INFO,
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
int BasicV1Client::getMsgInspectPeriodMsec() const{
    return inspectTimer.interval();
}
void BasicV1Client::setMsgInspectPeriodMsec(int value){
    inspectTimer.setInterval(value);
}
uint BasicV1Client::getMsgMaxAtemptToDelete() const{
    return msgMaxAtemptToDelete;
}
void BasicV1Client::setMsgMaxAtemptToDelete(uint value){
    msgMaxAtemptToDelete = value;
}
uint BasicV1Client::getMsgTransmitRepeatSec() const{
    return msgTransmitRepeatSec;
}
void BasicV1Client::setMsgTransmitRepeatSec(uint value){
    msgTransmitRepeatSec = value;
}






