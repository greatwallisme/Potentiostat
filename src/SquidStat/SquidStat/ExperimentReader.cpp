#include "ExperimentReader.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#define BUILDER_CONTAINER_SET		"set"
#define BUILDER_CONTAINER_ELEMENT	"element"

#define FIND_VALUE(name, check)													\
	if (joEnd == (joIt = jo.constFind(name))) {									\
		throw QString("There is no required field \"" name "\"");				\
	}																			\
	if(!joIt->check()) {														\
		throw QString("Field \"" name "\" has incorrect type (" #check ")");	\
	}

#define FILL_VALUE_OPTIONAL(name, check, var, convert)							\
	if (joEnd != (joIt = jo.constFind(name))) {									\
		if (!joIt->check()) {													\
			throw QString("Field \"" name "\" has incorrect type (" #check ")");\
		}																		\
		var = joIt->convert();													\
	}

#define JSON_UUID			"uuid"
#define JSON_NAME			"name"
#define JSON_TYPE			"type"
#define JSON_ELEMENTS		"elements"
#define JSON_REPEATS		"repeats"
#define JSON_PLUGIN_NAME	"plugin-name"
#define JSON_USER_INPUT		"user-input"

UserInput ParseUserInputJson(const QJsonObject &jo) {
	UserInput ret;

	foreach(auto &val, jo) {
		if (val.isArray()) {
			throw QString("User Input contains an array value, but it should not.");
		}
		if (val.isObject()) {
			throw QString("User Input contains an object value, but it should not.");
		}
	}

	ret = jo.toVariantMap();

	return ret;
}
BuilderContainer ParseContainerJson(const QJsonObject &jo) {
	BuilderContainer ret;
	ret.id = QUuid::createUuid();
	ret.w = 0;
	ret.elem.ptr = 0;

	auto joEnd = jo.constEnd();
	auto joIt = jo.constBegin();

	FIND_VALUE(JSON_REPEATS, isDouble);
	ret.repetition = joIt->toInt();

	FIND_VALUE(JSON_TYPE, isString);
	auto type = joIt->toString();

	if (type == BUILDER_CONTAINER_ELEMENT) {
		ret.type = BuilderContainer::ELEMENT;
		
		FIND_VALUE(JSON_PLUGIN_NAME, isString);
		ret.elem.name = joIt->toString();

		FIND_VALUE(JSON_USER_INPUT, isObject);
		ret.elem.input = ParseUserInputJson(joIt->toObject());
	}
	if (type == BUILDER_CONTAINER_SET) {
		ret.type = BuilderContainer::SET;

		FIND_VALUE(JSON_ELEMENTS, isArray);
		foreach(const QJsonValue &val, joIt->toArray()) {
			if (!val.isObject()) {
				throw QString("Set contains non-object value");
			}

			ret.elements << ParseContainerJson(val.toObject());
		}
	}

	return ret;
}
CustomExperiment ExperimentReader::GenerateExperimentContainer(const QByteArray &jsonData) {
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

	auto jo = jdoc.object();

	auto joEnd = jo.constEnd();
	auto joIt = jo.constBegin();

	CustomExperiment ce;

	FIND_VALUE(JSON_NAME, isString);
	ce.name = joIt->toString();

	FIND_VALUE(JSON_UUID, isString);
	ce.id = QUuid(joIt->toString());

	FIND_VALUE(JSON_ELEMENTS, isObject);

	ce.bc = ParseContainerJson(joIt->toObject());

	return ce;
}

QJsonObject GenerateObjectForUserInput(const UserInput &inputs) {
	QJsonObject ret;
	
	for (auto it = inputs.begin(); it != inputs.end(); ++it) {
		switch (it.value().type()) {
			case QVariant::Bool:
				ret.insert(it.key(), it.value().toBool());
				break;

			case QVariant::String:
				ret.insert(it.key(), it.value().toString());
				break;

			case QVariant::Int:
				ret.insert(it.key(), it.value().toInt());
				break;

			case QVariant::Double:
				ret.insert(it.key(), it.value().toDouble());
				break;

			default:
				break;
		}
	}
	
	return ret;
}
QJsonObject GenerateObjectForContainer(const BuilderContainer &bc) {
	QJsonObject ret;

	ret.insert(JSON_REPEATS, bc.repetition);

	switch (bc.type) {
		case BuilderContainer::ELEMENT:
			ret.insert(JSON_TYPE, BUILDER_CONTAINER_ELEMENT);
			ret.insert(JSON_PLUGIN_NAME, bc.elem.name);
			ret.insert(JSON_USER_INPUT, GenerateObjectForUserInput(bc.elem.input));
			break;
	
		case BuilderContainer::SET: {
			ret.insert(JSON_TYPE, BUILDER_CONTAINER_SET);
			QJsonArray jArray;
			for (auto it = bc.elements.begin(); it != bc.elements.end(); ++it) {
				jArray.append(GenerateObjectForContainer(*it));
			}
			ret.insert(JSON_ELEMENTS, jArray);
		} break;
	}

	return ret;
}
QByteArray ExperimentWriter::GenerateJsonObject(const CustomExperiment &ce) {
	QJsonDocument jDoc;

	QJsonObject joExp;

	joExp.insert(JSON_NAME, ce.name);
	joExp.insert(JSON_UUID, ce.id.toString());
	joExp.insert(JSON_ELEMENTS, GenerateObjectForContainer(ce.bc));

	jDoc.setObject(joExp);

	return jDoc.toJson();
}
