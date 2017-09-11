TEMPLATE = lib
TARGET = Chronopotentiometry
DESTDIR = ../../../out/Release/prebuilt
QT += core widgets gui
CONFIG += release plugin
DEFINES += QT_DESIGNER_LIB QT_WIDGETS_LIB
INCLUDEPATH += ./../../../../_arduino/retrofit_firmware \
    ./../../SquidStat \
    ./GeneratedFiles \
    . \
    ./GeneratedFiles/Release
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(Chronopotentiometry.pri)
