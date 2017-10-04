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
NetCommander::NetCommander(NetTransport *transport, QObject *parent) :
    Eventful (parent), transport(transport)
{

}
void NetCommander::setAuthData(const AuthData &auth)
{
    this->auth = auth;
}


/******************** Implementation *********************/
/***** NetTransport ******/
