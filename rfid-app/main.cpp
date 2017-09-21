#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QtQml>
#include <QtSerialPort/QSerialPortInfo>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);


    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
