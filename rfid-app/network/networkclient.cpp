#include "networkclient.h"


/********************** Interface ************************/

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
void NetCommander::transportStateChanged(NetState newState)
{
//    if(newState.state == QAbstractSocket::ConnectedState)
//        setstate(CONNECTED);
//    else
//        setstate(DISCONNECTED);
}

void NetCommander::setState(NetCommanderState state)
{
    this->state = state;
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
    NetCommander(transport,protocol,parent), addr(addr), mode(POOL){
}
void BasicV1Client::netEventOut(QSharedPointer<Event>)
{
}

void BasicV1Client::start()
{
//    QMetaObject::invokeMethod(phy.data(), "connectToHost",  Qt::QueuedConnection,
//                                  Q_ARG(NetPoint, addr));
    emit connectToHost(addr);
}

void BasicV1Client::stop()
{
//    QMetaObject::invokeMethod(phy.data(), "disconnectFromHost",  Qt::QueuedConnection);
    emit disconnectFromHost();
}

void BasicV1Client::recv(QByteArray data)
{
    emit send(data);
}


/*
QJsonObject AuthData::toJson() const
{
    QJsonObject json;
    json["user"] = user;
    json["pass"] = pass;
    return json;
}*/
