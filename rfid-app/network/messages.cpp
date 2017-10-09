#include "messages.h"


NetMessage::NetMessage(NetMessage::MsgID id, QJsonObject payload) :
    NetMessage(id,payload,QUuid::createUuid())
{
}

NetMessage::NetMessage(NetMessage::MsgID id, QJsonObject payload, QUuid uuid) :
    msgid(id), payload(payload), uuid(uuid), transmitCount(0)
{
}

QByteArray NetMessage::pack(const QAuthenticator &auth)
{
    transmitCount++;
    lastTransmit = QDateTime::currentDateTime();

}

uint NetMessage::getTransmitCount() const
{
    return transmitCount;
}

QDateTime NetMessage::getLastTransmit() const
{
    return lastTransmit;
}
