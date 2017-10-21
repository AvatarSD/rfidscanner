#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "common/facade.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    /********************************/
    QQmlApplicationEngine engine;
    
    /********************************/
    QScopedPointer<ScannerFacade> facade(new ScannerFacade(&app));
    engine.rootContext()->setContextProperty("facade", facade.data());
    
    /********************************/
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    /********************************/
    facade->setServer("localhost");
    facade->setPort(5600);
    facade->setUsername("denchick");
    facade->setPassword("qwerty88");
    facade->setMode(NetClientWorkMode::EVENT);

    // testing
    facade->connectToServer();

    /********************************/
    return app.exec();
}
