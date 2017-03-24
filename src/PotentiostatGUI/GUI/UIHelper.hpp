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

#define tr(...)         QObject::tr(__VA_ARGS__)
#define connect(...)    QObject::connect(__VA_ARGS__)

#define LBL(a)          new QLabel(tr(a))
#define S_LBL(a)        SizedWidget(new QLabel(tr(a)), GetSmaller)
#define B_LBL(a)        SizedWidget(new QLabel(tr(a)), GetBigger)
#define S_CKB(a)        SizedWidget(new QCheckBox(tr(a)), GetSmaller)
#define S_LED(a)        SetMinimumSizePolicy(SizedWidget(new QLineEdit(tr(a)), GetSmaller))
#define BOLD(a)         FontFamilyWidget(a, GetBold)
#define ITALIC(a)       FontStyleWidget(a, GetItalic)

#define B_PBT(a)        SizedWidget(new QPushButton(tr(a)), GetBigger)
#define S_PBT(a)        SizedWidget(new QPushButton(tr(a)), GetSmaller)
#define PBT(a)          new QPushButton(tr(a))
#define WDG()			new QWidget

template<typename WidgetType>
WidgetType* SetText(WidgetType *w, const QString &text) {
    w->setText(text);
    return w;
}
#define TOOL_BT(a)      SetText(new QToolButton, tr(a))

#define TXT_CNTR(a)     SetHCenterAlignment(a)

#define OBJ_NAME(w, str) SetObjectName(w, str)
#define OBJ_PROP(w, name, val) SetObjectProperty(w, name, val)
