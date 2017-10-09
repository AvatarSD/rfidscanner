#include "networkclient.h"


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
    NetCommander(transport,protocol,parent), addr(addr), mode(POOL){
    connect(this, SIGNAL(stateChanged(NetCommanderState)),
            this, SLOT(stateChanged(NetCommanderState)));
}
void BasicV1Client::start(){
    emit connectToHost(addr);
}
void BasicV1Client::stop(){
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


void BasicV1Client::netEventOut(QSharedPointer<Event> event){
    QSharedPointer<NetMessage> newMsg(event->event == Event::TAG ?
                static_cast<NetMessage*>(new TagEventMsg(event->toJson())) :
                static_cast<NetMessage*>(new ErrEventMsg(event->toJson())));
    messageQueue.enqueue(newMsg);
    if(mode == EVENT)
        emit transmitMsg(newMsg->pack(auth));

}
void BasicV1Client::receiveMsg(QByteArray data){

}

void BasicV1Client::stateChanged(NetCommanderState state){

}


/*
QJsonObject AuthData::toJson() const
{
    QJsonObject json;
    json["user"] = user;
    json["pass"] = pass;
    return json;
}*/


