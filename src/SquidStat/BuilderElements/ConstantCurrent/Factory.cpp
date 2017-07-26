#include "Factory.h"
#include "ConstCurrentElement.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new ConstCurrentElement;
}