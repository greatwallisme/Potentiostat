# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = MaxPower
DESTDIR = ../../../../out/Release/prebuilt
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
include(MaxPower.pri)
