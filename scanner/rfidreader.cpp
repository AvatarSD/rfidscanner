#include "rfidreader.h"

RFIDReader::RFIDReader()
{

}

bool RFIDReader::getFullMem(Tag &tag)
{
    return (getEPCMem(tag) && getUserMem(tag) && getReserveMem(tag));
}

void ReaderError::writeToJson(QJsonObject &json) const
{

}

QString ReaderError::toString() const
{

}

ReaderError::ErrorType ReaderError::getErrorType() const
{
    return (ErrorType)-1;
}
