#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "common/facade.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    /********************************/
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    /********************************/
    QScopedPointer<ScannerFacade> facade(new ScannerFacade);
    
    QThread facadeThread;
    facade->moveToThread(&facadeThread);
    facadeThread.start();
    
    /********************************/
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&]{
        facadeThread.quit();
        facadeThread.wait();
    });
    
    /********************************/
    engine.rootContext()->setContextProperty("facade", facade.data());
    
    /********************************/
    facade->setServer("localhost");
    facade->setPort(5600);
    facade->setUsername("denchick");
    facade->setPassword("qwerty88");
    facade->setMode(NetClient::EVENT);

    // testing
    facade->connectToServer();

    /********************************/
    return app.exec();
}
