HEADERS += ./InstrumentStructures.h \
    ./InstrumentEnumerator.h \
    ./Config.h \
    ./SerialThread.h \
    ./SerialCommunicator.h \
    ./ExperimentCalcHelper.h \
    ./ExternalStructures.h \
    ./InstrumentOperator.h \
    ./Disconnector.h \
    ./ListSeriesData.h \
    ./UIHelper.hpp \
    ./BuilderWidget.h \
    ./UIEventFilters.hpp \
    ./MainWindowUI.h \
    ./MainWindow.h \
    ./Log.h \
    ./ExperimentReader.h \
    ./ExperimentUIHelper.h \
    ./ExperimentFactoryInterface.h \
    ./ManualExperimentRunner.h \
    ./AbstractExperiment.h \
    ./CustomExperimentRunner.h \
    ./ExperimentTrigger.hpp \
    ./AbstractBuilderElement.h \
    ./BuilderElementFactoryInterface.h \
    ./CrcCalculator.h \
    ./HexLoader.h \
    ./BootloaderOperator.h \
    ./HidCommunicator.h
SOURCES += ./main.cpp \
    ./InstrumentEnumerator.cpp \
    ./Config.cpp \
    ./SerialCommunicator.cpp \
    ./SerialThread.cpp \
    ./ExperimentCalcHelper.cpp \
    ./InstrumentOperator.cpp \
    ./BuilderWidget.cpp \
    ./ListSeriesData.cpp \
    ./MainWindow.cpp \
    ./MainWindowUI.cpp \
    ./Log.cpp \
    ./ExperimentReader.cpp \
    ./CustomExperimentRunner.cpp \
    ./ManualExperimentRunner.cpp \
    ./BootloaderOperator.cpp \
    ./CrcCalculator.cpp \
    ./HexLoader.cpp \
    ./HidCommunicator.cpp
RESOURCES += GUI.qrc
