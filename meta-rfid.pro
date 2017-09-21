TEMPLATE = subdirs

qusb.file = QUSB/src/src.pro

SUBDIRS = \
    rfid-app \
    qusb

rfid-app.depends += qusb
