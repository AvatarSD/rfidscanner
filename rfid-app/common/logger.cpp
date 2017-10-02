#include "logger.h"
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <string>

Logger::Logger() :
    serverLogLevel(InfoEvent::ERROR),
    fileLogLevel(InfoEvent::INFO),
    stdoutLogLevel(InfoEvent::DEBUG),
    logfilePath("log.txt")
{

}

InfoEvent::EventLevel Logger::getServerLogLevel() const
{
    return serverLogLevel;
}

void Logger::setServerLogLevel(const InfoEvent::EventLevel &value)
{
    serverLogLevel = value;
}

InfoEvent::EventLevel Logger::getFileLogLevel() const
{
    return fileLogLevel;
}

void Logger::setFileLogLevel(const InfoEvent::EventLevel &value)
{
    fileLogLevel = value;
}

InfoEvent::EventLevel Logger::getStdoutLogLevel() const
{
    return stdoutLogLevel;
}

void Logger::setStdoutLogLevel(const InfoEvent::EventLevel &value)
{
    stdoutLogLevel = value;
}

QString Logger::getLogfilePath() const
{
    return logfilePath;
}

void Logger::setLogfilePath(const QString &value)
{
    logfilePath = value;
}

void Logger::eventIn(QSharedPointer<Event> event)
{
    if(event->event == Event::TAG){
        toStdOut(event);
        writeToLogFile(event);
        emit netEventOut(event);
        return;
    }
    else if (event->event == Event::INF){
        InfoEvent* infoEvent;
        try{
            infoEvent = dynamic_cast<InfoEvent*>(event.data());
        }
        catch (const std::bad_cast& e){
            eventIn(QSharedPointer<SystemEvent> (new SystemEvent(
                                    SystemEvent::INFO,
                                    SystemEvent::LOGGER_DYNAMIC_CAST_ERR,
                QStringLiteral("Event object is not InfoEvent object"))));
        }
    if(infoEvent->level >= stdoutLogLevel)
        toStdOut(event);
    if(infoEvent->level >= fileLogLevel)
        writeToLogFile(event);
    if(infoEvent->level >= serverLogLevel)
        emit netEventOut(event);
    return;
    }
}

void Logger::writeToLogFile(QSharedPointer<Event> event)
{
    QFile logfile(logfilePath);
    if(!logfile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){
        eventIn(QSharedPointer<SystemEvent> (
                    new SystemEvent(SystemEvent::INFO,
                                    SystemEvent::LOGFILE_OPEN_ERR,
                                QStringLiteral("Cannot open log file to write"))));
        return;
    }
    QTextStream stream(&logfile);
    stream << event->toString() << "\r\n";
    logfile.close();

}

void Logger::toStdOut(QSharedPointer<Event> event)
{
    std::cout << event->toString().toStdString() << std::endl;
}
