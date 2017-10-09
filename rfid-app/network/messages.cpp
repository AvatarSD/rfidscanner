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

    QJsonObject json;
    json["auth"] = authDataToJson(auth);
    json["uuid"] = uuid.toString(); //todo: check!
    json["msgid"] = msgid;
    json["msg"] = payload;
    QJsonDocument doc(json);
    return doc.toJson();

return QByteArray("sdfasdf"); //todo
}


uint NetMessage::getTransmitCount() const
{
    return transmitCount;
}

QDateTime NetMessage::getLastTransmit() const
{
    return lastTransmit;
}

QJsonObject NetMessage::authDataToJson(const QAuthenticator &auth){
    QJsonObject json;
    json["user"] = auth.user();
    json["pass"] = auth.password();
    return json;
}
