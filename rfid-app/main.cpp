#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "common/scannerfacade.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

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
//    NetTransport * socket = ;
//    NetProtocol *proto = ;
    auto * net = new BasicV1Client(new TcpNetTransport,
                                           new NetProtocolV2Bound(
                                               NetProtocolFormat("$SD#", "\r\n\r\n")),
                                           NetPoint("localhost", 5600));
    net->setMode(BasicV1Client::EVENT);

    //System * sys = new System;

    /********************************/
    QThread netThread;
    net->moveToThread(&netThread);
    netThread.start();
    QThread logThread;
    logger->moveToThread(&logThread);
    logThread.start();

    /********************************/
    QObject::connect(&app, &QCoreApplication::aboutToQuit,[&](){
        logThread.quit();
        netThread.quit();
        logThread.wait();
        netThread.wait();});

    /********************************/
//    QObject::connect(socket, &NetTransport::recv, [&](QByteArray data)
//    {
//        //        auto tim = QTime::currentTime();
//        std::cout << "recved str: " << data.toStdString() << std::endl;
//        std::cout << "recved hex: " << data.toHex().toStdString() << std::endl;

//        auto packing = proto->pack(data);
//        std::cout << "packed str: " << packing.toStdString() << std::endl;
//        std::cout << "packed hex: " << packing.toHex().toStdString() << std::endl;

//        QByteArray unpacked;
//        for(int i = 0; i<packing.size(); i+=5)
//            unpacked += proto->parse(packing.mid(i,5));
//        socket->send(unpacked);
//        std::cout << "sended str: " << unpacked.toStdString() << std::endl;
//        std::cout << "sended hex: " << unpacked.toHex().toStdString() << std::endl;

//    });


    QObject::connect(net, SIGNAL(sysEvent(QSharedPointer<Event>)),
                     logger, SLOT(sysEventIn(QSharedPointer<Event>)));
//    QObject::connect(proto, SIGNAL(sysEvent(QSharedPointer<Event>)),
//                     logger, SLOT(sysEventIn(QSharedPointer<Event>)));
    QObject::connect(logger, SIGNAL(netEventOut(QSharedPointer<Event>)),
                     net,SLOT(netEventIn(QSharedPointer<Event>)));


    /********************************/
//    NetPoint("localhost", 5600);
//    QMetaObject::invokeMethod(socket, "connectToHost",  Qt::QueuedConnection,
//                              Q_ARG(NetPoint, np));
    net->start();

    /********************************/
    return app.exec();
}
