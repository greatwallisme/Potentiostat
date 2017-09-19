TEMPLATE = lib
TARGET = NormalPulseVoltammetry
DESTDIR = ../../../../out/Release/elements
QT += core widgets gui
CONFIG += release console
DEFINES += WIN64 QT_WIDGETS_LIB
INCLUDEPATH += ./../../../../_arduino/retrofit_firmware \
    ./../../SquidStat \
    . \
    ./GeneratedFiles/Release
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(NormalPulseVoltammetry.pri)
