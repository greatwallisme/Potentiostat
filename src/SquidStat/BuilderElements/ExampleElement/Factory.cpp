#include "Factory.h"

#include "ExampleElement.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new ExampleElement;
}