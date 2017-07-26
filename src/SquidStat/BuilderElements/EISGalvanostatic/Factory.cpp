#include "Factory.h"

#include "EISGalvanostatic.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new EISGalvanostatic;
}