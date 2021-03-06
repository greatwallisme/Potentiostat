#pragma once

#include <QObject>

#include <QLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QRadioButton>

template<typename WidgetType>
WidgetType* SetObjectName(WidgetType *w, const QString &name) {
	w->setObjectName(name);
	return w;
}
template<typename WidgetType>
WidgetType* SetObjectProperty(WidgetType *w, const char *name, const QString &value) {
	w->setProperty(name, value);
	return w;
}
template<typename LayoutType>
LayoutType* SetZeroMargin(LayoutType *l) {
	l->setContentsMargins(0, 0, 0, 0);
	return l;
}
template<typename LayoutType>
LayoutType* SetZeroSpacing(LayoutType *l) {
	l->setSpacing(0);
	return l;
}

#define TR(...)         QObject::tr(__VA_ARGS__)
#define CONNECT(...)    QObject::connect(__VA_ARGS__)



#define WDG()			new QWidget
#define LBL(str)		new QLabel(TR(str))
#define CMB()			new QComboBox
#define LED()			new QLineEdit
#define TED()			new QTextEdit
#define PBT(str)		new QPushButton(TR(str))
#define RBT(str)		new QRadioButton(TR(str))



#define OBJ_NAME(qobject, qstring)	SetObjectName(qobject, qstring)
#define OBJ_PROP(w, name, val)		SetObjectProperty(w, name, val)

#define NO_MARGIN(qlayout)	SetZeroMargin(qlayout)
#define NO_SPACING(qlayout)	SetZeroSpacing(qlayout)