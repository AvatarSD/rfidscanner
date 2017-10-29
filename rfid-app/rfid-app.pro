TEMPLATE = app subdirs

QT += qml quick
CONFIG += c++14

SOURCES += main.cpp \
    types.cpp \
    events.cpp \
    scanner/ConcreteScanners/chinecompacthidscanner.cpp \
    common/logger.cpp \
    common/system.cpp \
    network/a_phy.cpp \
    network/b_protocol.cpp \
    network/c_messages.cpp \
    network/d_client.cpp \
    common/facade.cpp \
    scanner/a_transport.cpp \
    scanner/c_scanner.cpp \
    scanner/b_commands.cpp \
    scanner/d_mamanger.cpp

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
    types.h \
    events.h \
    scanner/ConcreteScanners/chinecompacthidscanner.h \
    common/logger.h \
    common/system.h \
    network/a_phy.h \
    network/b_protocol.h \
    network/c_messages.h \
    network/d_client.h \
    common/facade.h \
    scanner/a_transport.h \
    scanner/c_scanner.h \
    scanner/b_commands.h \
    scanner/d_mamanger.h

#INCLUDEPATH += ../QUSB/src
#LIBS += -L../../build-meta-rfid-Desktop_Qt_5_9_1_GCC_64bit-Release/QUSB/lib -lqusb

