#include "networkclient.h"


/********************** Interface ************************/

/******* Auth Data *******/
AuthData::AuthData() : AuthData(QStringLiteral(""),QStringLiteral(""))
{

}
AuthData::AuthData(QString user, QString pass) :
    user(user), pass(pass)
{}
QString AuthData::getUser() const
{
    return user;
}
QString AuthData::getPass() const
{
    return pass;
}
void AuthData::setUser(const QString &value)
{
    user = value;
}

void AuthData::setPass(const QString &value)
{
    pass = value;
}
QJsonObject AuthData::toJson() const
{
    QJsonObject json;
    json["user"] = user;
    json["pass"] = pass;
    return json;
}

/***** NetCommander ******/
NetCommander::NetCommander(NetTransport* transport,
                           NetProtocol* protocol,
                           const NetPoint &addr,
                           QObject *parent) :
    Eventful (parent), phy(transport), proto(protocol), addr(addr),
    mode(POOL)//, state(DISCONNECTED)
{
    qRegisterMetaType<NetCommanderState>();

    proto->setParent(this);
    phy->setParent(nullptr);
    phy->moveToThread(&phyThread);

    proto->connectAsEventDrain(this);
    phy->connectAsEventDrain(this);

    connect(this, SIGNAL(send(QByteArray)), phy.data(), SLOT(send(QByteArray)));
    connect(phy.data(), SIGNAL(recv(QByteArray)),this,SLOT(recv(QByteArray)));
    connect(phy.data(), SIGNAL(stateChanged(NetState)),
            this,SLOT(transportStateChanged(NetState)));

    phyThread.start();
}

NetCommander::~NetCommander()
{
    phyThread.quit();
    phyThread.wait();
}
AuthData NetCommander::getAuth() const{
    return auth;
}
void NetCommander::setAuth(const AuthData &value){
    auth = value;
}
NetCommander::WorkMode NetCommander::getMode() const{
    return mode;
}
void NetCommander::setMode(WorkMode mode){
    mode = mode;
}
NetPoint NetCommander::getAddr() const{
    return addr;
}
void NetCommander::setAddr(const NetPoint &value){
    addr = value;
}
NetCommanderState NetCommander::getState() const{
    return state;
}
void NetCommander::transportStateChanged(NetState newState)
{
//    if(newState.state == QAbstractSocket::ConnectedState)
//        setstate(CONNECTED);
//    else
//        setstate(DISCONNECTED);
}

void NetCommander::setState(NetCommanderState state)
{
//    this->state = state;
//    if(state == AUTHENTICATED){
//        emit sysEvent(QSharedPointer<Event> (
//                          new NetworkEvent(NetworkEvent::INFO,
//                                           NetworkEvent::IDs::COMMANDER_STATE,
//                                           QStringLiteral("Authenticated like: ")+
//                                           auth.getUser())));
//        emit stateChanged(NetCommanderState state);
//    }
}


/******************** Implementation *********************/
/***** NetTransport ******/


BasicV1Client::BasicV1Client(NetTransport *transport,
                             NetProtocol *protocol,
                            const NetPoint &addr,
                             QObject *parent):
    NetCommander(transport,protocol,addr,parent)
{

}
BasicV1Client::~BasicV1Client()
{

}

void BasicV1Client::netEventOut(QSharedPointer<Event>)
{
}

void BasicV1Client::start()
{
    QMetaObject::invokeMethod(phy.data(), "connectToHost",  Qt::QueuedConnection,
                                  Q_ARG(NetPoint, addr));
}

void BasicV1Client::stop()
{
    QMetaObject::invokeMethod(phy.data(), "disconnectFromHost",  Qt::QueuedConnection);
}

void BasicV1Client::recv(QByteArray data)
{
}
