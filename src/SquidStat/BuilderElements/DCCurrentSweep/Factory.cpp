#include "Factory.h"

#include "ConstPotElementAdv.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new ConstPotElementAdv;
}