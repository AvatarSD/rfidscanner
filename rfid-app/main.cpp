#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "common/system.h"
#include "common/logger.h"
#include "scanner/rfidmamanger.h"
#include "network/networkclient.h"



int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);


    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;



    QObject * hmi = engine.rootContext()->contextObject();
    Logger * logger = new Logger;

   // PhyTransport * phy = new PhyTransport;
   // Protocol * prot = new Protocol;
   // RFIDReader * reader = new RFIDReader;
   // RFIDMamanger * mananger = new SimpleRFIDMamanger;

    NetTransport * socket = new SimpleTcpClient;
   // NetworkClient * net = new EventClient;

    //System * sys = new System;


    logger->eventIn(QSharedPointer<SystemEvent> (new SystemEvent(
                                                    SystemEvent::INFO,
                                                    SystemEvent::LOGGER_DYNAMIC_CAST_ERR,
                                                    QStringLiteral("Event object is not InfoEvent object"))));;

    logger->eventIn(QSharedPointer<SystemEvent> (new SystemEvent(
                                                    SystemEvent::WARNING,
                                                    SystemEvent::LOGFILE_OPEN_ERR,
                                                    QStringLiteral("Cannot open log file to write"))));




    return app.exec();
}
