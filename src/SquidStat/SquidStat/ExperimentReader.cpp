#include "ExperimentReader.h"

#include <QJsonDocument>
#include  <QJsonObject>

void ParseExperimentJson(const QJsonObject &jo, ExperimentContainer &ec) {
	auto joEnd = jo.constEnd();

	auto joIt = jo.constBegin();

	#define FIND_VALUE(name, check)									\
	if (joEnd == (joIt = jo.constFind(name))) {						\
		throw QString("There is no required field \"" name "\"");	\
	}																\
	if(!joIt->check()) {									\
		throw QString("Field has incorrect type");					\
	}

	FIND_VALUE("short-name", isString);
	ec.shortName = joIt->toString();
}

ExperimentContainer ExperimentReader::Generate(const QByteArray &jsonData) {
	QJsonParseError parseError;
	QJsonDocument jdoc = QJsonDocument::fromJson(jsonData, &parseError);

	if (QJsonParseError::NoError != parseError.error) {
		throw parseError.errorString();
	}

	if (jdoc.isNull()) {
		throw QString("QJsonDocument is NULL");
	}

	if (!jdoc.isObject()) {
		throw QString("QJsonDocument is NOT an object");
	}

	ExperimentContainer ec;

	ParseExperimentJson(jdoc.object(), ec);

	return ec;
}