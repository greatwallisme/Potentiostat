#include "Factory.h"

#include "ConstPotElement.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new ConstPotElement;
}