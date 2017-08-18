#pragma once

#include <QMetaType>
#include <ExternalStructures.h>

class QString;
class QByteArray;
class QPixmap;
class QWidget;
class QFile;

#include <QMap>
#include <QVector>

struct ExperimentNotes {
	QString description;
	QPair<QString, QString> refElectrode;
	struct {
		QPair<QString, QString> currentDensityWorkingElectrode;
		QPair<QString, QString> currentDensityCounterElectrode;
		QPair<QString, QString> solvent;
		QPair<QString, QString> electrolyte;
		QPair<QString, QString> electrolyteConcentration;
		QPair<QString, QString> atmosphere;
	} other;
};
#define COUNT_OF_EXPERIMENT_NOTES_LINES 8

typedef QList<qreal> DataList;
struct DataStore {
	DataStore() : min(0), max(0) {}
	DataList data;
	qreal min;
	qreal max;
};
typedef QMap<QString, DataStore> DataMap;

enum ExperimentType : uint8_t {
	ET_AC,
	ET_DC,
	ET_SAVED
};
typedef QList<ExperimentType> ExperimentTypeList;


#define PUSH_NEW_DC_DATA_DEFINITION \
	PushNewDcData(const ExperimentalDcData &expData, DataMap &container, const CalibrationData &calData, const HardwareVersion &hwVersion, const ExperimentNotes &notes) const

#define PUSH_NEW_AC_DATA_DEFINITION \
	PushNewAcData(const QByteArray &expDataRaw, DataMap &container, const CalibrationData &calData, const HardwareVersion &hwVersion, const ExperimentNotes &notes) const

class AbstractExperiment {
public:
	AbstractExperiment() {};

	virtual QString GetShortName() const = 0;
	virtual QString GetFullName() const = 0;
	virtual QString GetDescription() const = 0;
	virtual QStringList GetCategory() const = 0;
	virtual ExperimentTypeList GetTypes() const = 0;
	virtual QPixmap GetImage() const = 0;

	virtual QWidget* CreateUserInput() const = 0;
	virtual NodesData GetNodesData(QWidget*, const CalibrationData&, const HardwareVersion&) const = 0;

	virtual QStringList GetXAxisParameters(ExperimentType) const = 0;
	virtual QStringList GetYAxisParameters(ExperimentType) const = 0;
	
	virtual void PUSH_NEW_DC_DATA_DEFINITION {};
	virtual void SaveDcDataHeader(QFile&, const ExperimentNotes&) const {};
	virtual void SaveDcData(QFile&, const DataMap&) const {};

	virtual void PUSH_NEW_AC_DATA_DEFINITION {};
	virtual void SaveAcDataHeader(QFile&, const ExperimentNotes&) const {};
	virtual void SaveAcData(QFile&, const DataMap&) const {};
};

Q_DECLARE_METATYPE(AbstractExperiment*)
Q_DECLARE_METATYPE(const AbstractExperiment*)
