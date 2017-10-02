#ifndef SCANNERFACADE_H
#define SCANNERFACADE_H


class ScannerFacade
{
public:
    ScannerFacade();

    void setServerAddress();
    void setMode();
    void setAuth();
    void setScanner();
    void setLogFilePath();
    void setWifi();
    void setlogLevel();
};

#endif // SCANNERFACADE_H
