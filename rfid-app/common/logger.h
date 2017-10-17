#ifndef LOGGER_H
#define LOGGER_H

#include "events.h"
#include <QFile>
#include <QMutex>

class Logger : public QObject
{
    Q_OBJECT
public:
    Logger();

    InfoEvent::EventLevel getServerLogLevel() const;
    InfoEvent::EventLevel getFileLogLevel() const;
    InfoEvent::EventLevel getStdoutLogLevel() const;

    void setServerLogLevel(const InfoEvent::EventLevel &value);
    void setFileLogLevel(const InfoEvent::EventLevel &value);
    void setStdoutLogLevel(const InfoEvent::EventLevel &value);

    QString getLogfilePath() const;
    void setLogfilePath(QString value);

public slots:
    void sysEventIn(QSharedPointer<Event> event);

signals:
    void netEventOut(QSharedPointer<Event>);

private:
    void writeToLogFile(QSharedPointer<Event> event);
    void toStdOut(QSharedPointer<Event> event);

private:
    InfoEvent::EventLevel serverLogLevel;
    InfoEvent::EventLevel fileLogLevel;
    InfoEvent::EventLevel stdoutLogLevel;
    QString logfilePath;
    mutable QMutex pathAcces;
};

#endif // LOGGER_H
