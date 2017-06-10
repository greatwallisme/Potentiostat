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

typedef QVector<qreal> DataVector;
typedef QMap<QString, DataVector> DataMap;

class AbstractExperiment {
public:
	AbstractExperiment() {};

	virtual QString GetShortName() const = 0;
	virtual QString GetFullName() const = 0;
	virtual QString GetDescription() const = 0;
	virtual QStringList GetCategory() const = 0;
	virtual QPixmap GetImage() const = 0;

	virtual QWidget* CreateUserInput() const = 0;
	virtual QByteArray GetNodesData(QWidget*, const CalibrationData&) const = 0;

	virtual QStringList GetXAxisParameters() const = 0;
	virtual QStringList GetYAxisParameters() const = 0;
	virtual void PushNewData(const ExperimentalData&, DataMap &) const = 0;
	
	virtual void SaveDataHeader(QFile&) const = 0;
	virtual void SaveData(QFile&, const DataMap&) const = 0;
};

Q_DECLARE_METATYPE(AbstractExperiment*)
Q_DECLARE_METATYPE(const AbstractExperiment*)
