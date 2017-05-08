#pragma once

#include <QString>
#include <QWidget>

template<typename WidgetType, typename FontSizeOperator>
WidgetType* SizedWidget(WidgetType *w, FontSizeOperator oper) {
    w->setProperty("ng-font-size-policy", oper());
    return w;
}
template<typename WidgetType, typename FontSizeOperator>
WidgetType* FontFamilyWidget(WidgetType *w, FontSizeOperator oper) {
    w->setProperty("ng-font-family-policy", oper());
    return w;
}
template<typename WidgetType, typename FontSizeOperator>
WidgetType* FontStyleWidget(WidgetType *w, FontSizeOperator oper) {
    w->setProperty("ng-font-style-policy", oper());
    return w;
}
template<typename WidgetType>
WidgetType* SetMinimumSizePolicy(WidgetType *w) {
    w->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    return w;
}
template<typename WidgetType>
WidgetType* SetHCenterAlignment(WidgetType *w) {
    w->setAlignment(Qt::AlignHCenter);
    return w;
}
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
template<typename WidgetType>
WidgetType* SetCheckable(WidgetType *w) {
	w->setCheckable(true);
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

#define LBL(a)          new QLabel(TR(a))
#define S_LBL(a)        SizedWidget(new QLabel(TR(a)), GetSmaller)
#define B_LBL(a)        SizedWidget(new QLabel(TR(a)), GetBigger)
#define S_CKB(a)        SizedWidget(new QCheckBox(TR(a)), GetSmaller)
#define S_LED(a)        SetMinimumSizePolicy(SizedWidget(new QLineEdit(TR(a)), GetSmaller))
#define BOLD(a)         FontFamilyWidget(a, GetBold)
#define ITALIC(a)       FontStyleWidget(a, GetItalic)

#define B_PBT(a)        SizedWidget(new QPushButton(TR(a)), GetBigger)
#define S_PBT(a)        SizedWidget(new QPushButton(TR(a)), GetSmaller)
#define PBT(a)          new QPushButton(TR(a))
#define WDG()			new QWidget
#define LED(a)			new QLineEdit(TR(a))
#define CKB(a)			new QCheckBox(TR(a))

template<typename WidgetType>
WidgetType* SetText(WidgetType *w, const QString &text) {
    w->setText(text);
    return w;
}
#define TOOL_BT(a)      SetText(new QToolButton, TR(a))

#define TXT_CNTR(a)     SetHCenterAlignment(a)

#define OBJ_NAME(w, str) SetObjectName(w, str)
#define OBJ_PROP(w, name, val) SetObjectProperty(w, name, val)

#define NO_MARGIN(l)	SetZeroMargin(l)
#define NO_SPACING(l)	SetZeroSpacing(l)

#define CHEKABLE(w)		SetCheckable(w)