#ifndef COMMANDS_H
#define COMMANDS_H

#include "rfidprotocol.h"

/*********************** Commands ************************/

class InventoryRq : public ScannerRequest
{

};

class InventoryRp : public ScannerReply
{

};

#endif // COMMANDS_H