#include "ExperimentReader.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#define NODE_CONTAINER_SET		"set"
#define NODE_CONTAINER_NODE		"node"

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

NodeType_t GetNodeType(const QString &str) {
	#define CHECK_NODE_TYPE(type)	\
		if (str == #type) {			\
			return type;			\
		}
	
	CHECK_NODE_TYPE(DCNODE_OCP);
	CHECK_NODE_TYPE(DCNODE_SWEEP_POT);
	CHECK_NODE_TYPE(DCNODE_SWEEP_GALV);
	CHECK_NODE_TYPE(DCNODE_POINT_POT);
	CHECK_NODE_TYPE(DCNODE_POINT_GALV);
	CHECK_NODE_TYPE(DCNODE_NORMALPULSE_POT);
	CHECK_NODE_TYPE(DCNODE_NORMALPULSE_GALV);
	CHECK_NODE_TYPE(DCNODE_DIFFPULSE_POT);
	CHECK_NODE_TYPE(DCNODE_DIFFPULSE_GALV);
	CHECK_NODE_TYPE(DCNODE_SQRWAVE_POT);
	CHECK_NODE_TYPE(DCNODE_SQRWAVE_GALV);
	CHECK_NODE_TYPE(DCNODE_SINEWAVE);
	CHECK_NODE_TYPE(ACNODE_POTENTIOSTATIC);
	CHECK_NODE_TYPE(ACNODE_GALVANOSTATIC);
	CHECK_NODE_TYPE(ACNODE_HYBRID);

	throw QString("Field \"nodeType\" has incorrect type (") + str + QString(")");
}

void ReadNodeParameters(const QJsonObject &jo, ExperimentNode_t &node) {
	auto joEnd = jo.constEnd();
	auto joIt = jo.constBegin();

	FIND_VALUE("nodeType", isString);
	node.nodeType = GetNodeType(joIt->toString());

	/*
	FILL_VALUE_OPTIONAL("VStart", isDouble, node.DCSweep.VStart, toInt);
	FILL_VALUE_OPTIONAL("VEnd", isDouble, node.DCSweep.VEnd, toInt);
	FILL_VALUE_OPTIONAL("dVdt", isDouble, node.DCSweep.dVdt, toDouble);
	//*/
}

void ReadNodes(const QJsonObject &jo, NodeContainer &nc) {
	auto joEnd = jo.constEnd();
	auto joIt = jo.constBegin();

	FIND_VALUE("repetition", isDouble);
	nc.repetition = joIt->toInt();

	FIND_VALUE("type", isString);
	QString type = joIt->toString();

	if (type == NODE_CONTAINER_SET) {
		nc.type = NodeContainer::SET;
	}
	else if (type == NODE_CONTAINER_NODE) {
		nc.type = NodeContainer::NODE;
	}
	else {
		throw QString("Unknown type of the node container (") + type + QString(")");
	}

	switch (nc.type) {
		case NodeContainer::NODE:
			FIND_VALUE("parameters", isObject);
			ReadNodeParameters(joIt->toObject(), nc.parameters);
			break;

		case NodeContainer::SET:
			FIND_VALUE("elements", isArray);
			foreach(const QJsonValue &val, joIt->toArray()) {
				if (!val.isObject()) {
					throw QString("Elements contains non-object value");
				}

				NodeContainer childNc;
				ReadNodes(val.toObject(), childNc);
				nc.elements << childNc;
			}
			break;
	}
}

void ParseExperimentJson(const QJsonObject &jo, ExperimentContainer &ec) {
	auto joEnd = jo.constEnd();
	auto joIt = jo.constBegin();

	FIND_VALUE("short-name", isString);
	ec.shortName = joIt->toString();

	FIND_VALUE("name", isString);
	ec.name = joIt->toString();

	FIND_VALUE("description", isString);
	ec.description = joIt->toString();

	FIND_VALUE("image-path", isString);
	ec.imagePath = joIt->toString();

	FIND_VALUE("nodes", isObject);
	ReadNodes(joIt->toObject(), ec.nodes);
}

ExperimentContainer ExperimentReader::GenerateExperimentContainer(const QByteArray &jsonData) {
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
void AddNodePointers(QList<ExperimentNode_t*> &nodePtrs, NodeContainer &nc) {
	switch (nc.type) {
		case NodeContainer::NODE:
			nodePtrs << &nc.parameters;
			break;
		case NodeContainer::SET:
			for (auto it = nc.elements.begin(); it != nc.elements.end(); ++it) {
				AddNodePointers(nodePtrs, *it);
			}
			break;
	}
}
QList<ExperimentNode_t*> ExperimentReader::GetNodeListForUserInput(ExperimentContainer &ec) {
	QList<ExperimentNode_t*> ret;

	AddNodePointers(ret, ec.nodes);

	return ret;
}

void FillDcNodeSweep(ExperimentNode_t &node) {
	/*
	node.isHead = false;
	node.isTail = false;
	node.nodeType = DCNODE_SWEEP;
	node.tMin = 100000;
	node.tMax = 10000000000;
	node.samplingParams.ADCTimerDiv = 2;
	node.samplingParams.ADCTimerPeriod = 15625;
	node.samplingParams.ADCBufferSize = 20;
	node.samplingParams.DACMultiplier = 20;
	//*/
}
void FillNodeParameters(ExperimentNode_t &node) {
	switch (node.nodeType) {
		/*
		case DCNODE_SWEEP:
			FillDcNodeSweep(node);
			break;
		//*/

		default:
			break;
	}
}
void AddNodeInstances(QVector<ExperimentNode_t> &nodes, NodeContainer &nc) {
	int repetition = nc.repetition;
	switch (nc.type) {
		case NodeContainer::NODE:
			FillNodeParameters(nc.parameters);
			while (repetition--) {
				nodes << nc.parameters;
			}
			break;
		case NodeContainer::SET: {
				QVector<ExperimentNode_t> branch;
				for (auto it = nc.elements.begin(); it != nc.elements.end(); ++it) {
					AddNodeInstances(branch, *it);
				}
				while (repetition--) {
					nodes << branch;
				}
			}
			break;
	}
}
QVector<ExperimentNode_t> ExperimentReader::GetNodeArrayForInstrument(ExperimentContainer &ec) {
	QVector<ExperimentNode_t> ret;

	AddNodeInstances(ret, ec.nodes);

	ExperimentNode_t endNode;
	endNode.nodeType = END_EXPERIMENT_NODE;

	ret << endNode;

	return ret;
}