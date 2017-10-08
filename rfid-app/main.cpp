#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "common/system.h"
#include "common/logger.h"
#include "scanner/rfidmamanger.h"
#include "network/networkclient.h"

#include <iostream>

Q_DECLARE_METATYPE(QSharedPointer<Event>)

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    /********************************/
    qRegisterMetaType<QSharedPointer<Event>>();

    /********************************/
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;


    /********************************/
    //    QObject * hmi = engine.rootContext()->contextObject();
    Logger * logger = new Logger;

    // PhyTransport * phy = new PhyTransport;
    // Protocol * prot = new Protocol;
    // RFIDReader * reader = new RFIDReader;
    // RFIDMamanger * mananger = new SimpleRFIDMamanger;
    NetTransport * socket = new TcpNetTransport;
    // NetCommander * net = new EventClient;

    //System * sys = new System;
    NetProtocol *proto = new NetProtocolV2Bound(
                NetProtocolFormat("$SD#", "\r\n\r\n"));

    /********************************/
    QThread netThread;
    socket->moveToThread(&netThread);
    netThread.start();
    QThread logThread;
    logger->moveToThread(&logThread);
    logThread.start();
    //    QObject::connect(&app, SIGNAL(aboutToQuit()),&logThread,SLOT(terminate()));

    /********************************/
    QObject::connect(&app, &QCoreApplication::aboutToQuit,[&](){
        logThread.quit();
        netThread.quit();
        logThread.wait();
        netThread.wait();});

    /********************************/
    QObject::connect(socket, &NetTransport::recv, [&](QByteArray data){
       NetProtocol::NetProtocolParseErr err;
//       std::cout << "sended: " << socket->send(proto->pack(data)) <<
       std::cout << "sended: " << socket->send(proto->parse(proto->pack(data), &err)) <<
                     " byte(s); err: ";
        std::cout << (uint32_t)err << std::endl;
    });

    QObject::connect(socket, &NetTransport::recv, [&](QByteArray data){
        std::cout << "socket data: " << data.toStdString() << std::endl;
    });
    QObject::connect(socket, SIGNAL(sysEvent(QSharedPointer<Event>)),
                     logger, SLOT(sysEventIn(QSharedPointer<Event>)));


    /********************************/
    logger->sysEventIn(QSharedPointer<Event> (new SystemEvent(
                                                  SystemEvent::INFO,
                                                  SystemEvent::LOGGER_DYNAMIC_CAST_ERR,
                                                  QStringLiteral("Event object is not InfoEvent object"))));;

    logger->sysEventIn(QSharedPointer<Event> (new SystemEvent(
                                                  SystemEvent::WARNING,
                                                  SystemEvent::LOGFILE_OPEN_ERR,
                                                  QStringLiteral("Cannot open log file to write"))));


    /********************************/
    NetPoint np("localhost", 5600);
    QMetaObject::invokeMethod(socket, "connectToHost",  Qt::QueuedConnection,
                              Q_ARG(NetPoint, np));

    /********************************/
    return app.exec();
}
