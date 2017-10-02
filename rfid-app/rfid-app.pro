TEMPLATE = app subdirs

QT += qml quick
CONFIG += c++14

SOURCES += main.cpp \
    scanner/rfidmamanger.cpp \
    scanner/rfidreader.cpp \
    types.cpp \
    network/networkclient.cpp \
    network/transportiface.cpp \
    events.cpp \
    scanner/transport.cpp \
    scanner/rfidprotocol.cpp \
    scanner/ConcreteScanners/chinecompacthidscanner.cpp \
    scanner/commands.cpp \
    network/messages.cpp \
    system/scannerfacade.cpp \
    system/logger.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
target.path = /home/pi/scanner
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    scanner/rfidmamanger.h \
    scanner/rfidreader.h \
    types.h \
    network/networkclient.h \
    network/transportiface.h \
    events.h \
    scanner/transport.h \
    scanner/rfidprotocol.h \
    scanner/ConcreteScanners/chinecompacthidscanner.h \
    scanner/commands.h \
    network/messages.h \
    system/scannerfacade.h \
    system/logger.h

INCLUDEPATH += ../QUSB/src
LIBS += -L../../build-meta-rfid-Desktop_Qt_5_9_1_GCC_64bit-Release/QUSB/lib -lqusb

