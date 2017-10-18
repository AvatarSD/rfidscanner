#include "c_messages.h"


/********************** Level 3(C) ***********************/

NetMessage::NetMessage(NetMessage::MsgID id, QJsonObject payload) :
    NetMessage(id,payload,QUuid::createUuid()){
}
NetMessage::NetMessage(NetMessage::MsgID id, QJsonObject payload, QUuid uuid) :
    msgid(id), payload(payload), uuid(uuid), transmitCount(0),
    lastTransmit(QDateTime::currentDateTime()){
}
QByteArray NetMessage::pack(const QAuthenticator &auth)
{
    transmitCount++;
    lastTransmit = QDateTime::currentDateTime();

    QJsonObject json;
    json["auth"] = authDataToJson(auth);
    json["uuid"] = uuid.toString();
    json["msgid"] = msgid;
    json["msg"] = payload;
    QJsonDocument doc(json);
    return doc.toJson();
}
uint NetMessage::getTransmitCount() const{
    return transmitCount;
}
QDateTime NetMessage::getLastTransmit() const{
    return lastTransmit;
}
QJsonObject NetMessage::authDataToJson(const QAuthenticator &auth){
    QJsonObject json;
    json["user"] = auth.user();
    json["pass"] = auth.password();
    return json;
}

QSharedPointer<NetMessage> NetMessage::createMsg(MsgID id, QJsonObject payload, QUuid uuid)
{
//    switch (id) {
//    case TAG_EVENT: return new TagEventMsg(payload,uuid);
//    ERROR_EVENT,
//    ANSWER,
//    REQUEST,
//    SCANNER_RUNTIME,
//    EVENT_COUNT:

//        break;
//    default:
//        break;
//    }
}
QSharedPointer<NetMessage> NetMessage::parse(QByteArray data, ParseError *err)
{
//    QJsonDocument doc = QJsonDocument::fromJson(data, &err->jsonError);
//    if(err->jsonError != QJsonParseError::NoError){
//        err->status = ParseError::JSON_ERR;
//        return nullptr;
//    }
//    if(!doc.isObject()){
//        err->status = ParseError::CONTENT_ERR;
//        return nullptr;
//    }
//    QJsonObject json = doc.object();
//    if(!(json.contains("msgid")&&json.contains("msg")&&json.contains("uuid"))){
//        err->status = ParseError::CONTENT_ERR;
//        return nullptr;
//    }
}
