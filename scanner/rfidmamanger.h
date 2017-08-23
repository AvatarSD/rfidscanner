#ifndef RFIDMAMANGER_H
#define RFIDMAMANGER_H

#include "rfidreader.h"

#include <QSharedPointer>
#include <QList>

#include "types.h"

class RFIDMamanger
{
public:
    typedef QList<QSharedPointer<LogEvent>> LogList;

    RFIDMamanger(RFIDReader * reader);
    ~RFIDMamanger();

    virtual void run(){}
    const LogList & getRuntimeList();

private:
    LogList logs;
    RFIDReader * reader;

};


class SimpleRFIDMamanger : public RFIDMamanger
{
    // RFIDMamanger interface
public:
    void run() final;
};



#endif // RFIDMAMANGER_H
