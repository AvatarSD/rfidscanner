#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <QObject>
#include <QThread>

class Transport : public QObject
{
    Q_OBJECT
public:
    explicit Transport(QObject *parent = nullptr);

signals:

public slots:

protected:
    QThread thread;
};

#endif // TRANSPORT_H
