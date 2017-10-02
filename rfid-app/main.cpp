#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QtQml>
//#include <QtSerialPort/QSerialPortInfo

#include "common/logger.h"




int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);


    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;


    Logger              logger;
    logger.eventIn(QSharedPointer<SystemEvent> (new SystemEvent(
                                                    SystemEvent::INFO,
                                                    SystemEvent::LOGGER_DYNAMIC_CAST_ERR,
                                                    QStringLiteral("Event object is not InfoEvent object"))));;

    logger.eventIn(QSharedPointer<SystemEvent> (new SystemEvent(
                                                    SystemEvent::WARNING,
                                                    SystemEvent::LOGFILE_OPEN_ERR,
                                                    QStringLiteral("Cannot open log file to write"))));




    return app.exec();
}
