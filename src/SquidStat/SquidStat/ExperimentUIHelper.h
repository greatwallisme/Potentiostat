#pragma once

#include <QString>
#include <QPixmap>
#include <QButtonGroup>
#include <QListView>

#include <QWidget>

#include <QHBoxLayout>
#include <QSettings>

#include <QFile>

#include <UIHelper.hpp>

#define EXPERIMENT_VALUES_INI	 "ExperimentValues.ini"

class Disconnector : public QObject {
public:
	Disconnector(QObject *parent) : QObject(parent) {}
	~Disconnector() {
		foreach(auto c, connections) {
			disconnect(c);
		}
	}

	Disconnector& operator <<(const QMetaObject::Connection &c) {
		connections << c;
		return *this;
	}

private:
	QList<QMetaObject::Connection> connections;
};

#define GET_DATA_PTR(expDataRaw) \
	(ExperimentalAcData*)expDataRaw.data()
#define GET_DATA_ARRAY_SIZE(expDataRaw) \
	(expDataRaw.size() - sizeof(ExperimentalAcData)) / sizeof(int16_t)

#define PUSH_BACK_DATA(store, val) {		\
	if (container[store].data.isEmpty()) {	\
		container[store].min = val;			\
		container[store].max = val;			\
	}										\
	else {									\
		if (val > container[store].max) {	\
			container[store].max = val;		\
		}									\
		if (val < container[store].min) {	\
			container[store].min = val;		\
		}									\
	}										\
	container[store].data.push_back(val);	\
}

#define SAVE_DATA_HEADER_START()	\
	QString headers;				\
	QString axes;					\
	QString prevChar = "";			\
	QString str;

#define SAVE_DATA_HEADER(varName)	\
	headers += prevChar;			\
	axes += prevChar;				\
	headers += QString("\"%1\"").arg(QString(varName).replace("\"", "\"\""));	\
	str = GetXAxisParameters().contains(varName) ? "X" : "";					\
	str += GetYAxisParameters().contains(varName) ? "Y" : "";					\
	axes += QString("\"%1\"").arg(str);											\
	prevChar = ";";

#define SAVE_DATA_HEADER_END()			\
	saveFile.write(headers.toLatin1()); \
	saveFile.write("\n");				\
	saveFile.write(axes.toLatin1());	\
	saveFile.write("\n");				\
	saveFile.flush();


#define SAVE_DATA_START() \
	static QChar decimalPoint = QLocale().decimalPoint(); \
	QString prevChar = "";

#define SAVE_DATA(varName)	\
	saveFile.write(prevChar.toLatin1()); \
	saveFile.write(QString("%1").arg(container[varName].data.last(), 0, 'e').replace(QChar('.'), decimalPoint).toLatin1()); \
	prevChar = ";";

#define SAVE_DATA_END() \
	saveFile.write("\n"); \
	saveFile.flush();

#define _INSERT_RIGHT_ALIGN_COMMENT(text, row, col) \
	{ \
		auto lbl = OBJ_PROP(OBJ_NAME(LBL(text), "experiment-params-comment"), "comment-placement", "left"); \
		OBJ_PROP(lbl, "experiment-params-widget", "low-margin"); \
		lay->addWidget(lbl, row, col); \
	}

#define _INSERT_LEFT_ALIGN_COMMENT(text, row, col) \
	{ \
		auto lbl = OBJ_PROP(OBJ_NAME(LBL(text), "experiment-params-comment"), "comment-placement", "right"); \
		OBJ_PROP(lbl, "experiment-params-widget", "low-margin"); \
		lay->addWidget(lbl, row, col); \
	}

#define _INSERT_TEXT_INPUT(default_value, obj_name, row, col) \
	{	\
		auto led = OBJ_NAME(new QLineEdit(), obj_name); \
		led->setText(settings.value(obj_name, default_value).toString()); \
		OBJ_PROP(led, "experiment-params-widget", "low-margin"); \
		lay->addWidget(led, row, col); \
		*diconnector << CONNECT(led, &QLineEdit::textChanged, [=](const QString &str) { \
			QSettings localSettings(EXPERIMENT_VALUES_INI, QSettings::IniFormat); \
			localSettings.beginGroup(GetFullName()); \
			localSettings.setValue(obj_name, str); \
		}); \
	}

#define _START_RADIO_BUTTON_GROUP(obj_name)	\
	{										\
		auto group = new QButtonGroup(ret);	\
		OBJ_NAME(group, obj_name);

#define _END_RADIO_BUTTON_GROUP()				\
		auto val = settings.value(group->objectName(), "").toString();\
		foreach(auto rbt, group->buttons()) {	\
			if(rbt->text() == val)	{			\
				rbt->setChecked(true);			\
				break;							\
			}									\
		}										\
		*diconnector << CONNECT(group, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), [=](QAbstractButton *button) { \
			QSettings localSettings(EXPERIMENT_VALUES_INI, QSettings::IniFormat); \
			localSettings.beginGroup(GetFullName()); \
			localSettings.setValue(group->objectName(), button->text()); \
		});										\
	}

#define _INSERT_RADIO_BUTTON(text, row, col)	\
	{											\
		auto button = RBT(text);				\
		OBJ_PROP(button, "experiment-params-widget", "low-margin"); \
		group->addButton(button);				\
		if(0 == group->checkedButton()) {		\
			button->setChecked(true);			\
		}										\
		lay->addWidget(button, row, col);		\
	}

#define _START_RADIO_BUTTON_GROUP_HORIZONTAL_LAYOUT(obj_name, row, col)	\
	_START_RADIO_BUTTON_GROUP(obj_name);						\
	auto butLay = NO_SPACING(NO_MARGIN(new QHBoxLayout));		\
	lay->addLayout(butLay, row, col);

#define _END_RADIO_BUTTON_GROUP_LAYOUT()		\
		butLay->addStretch(1);					\
		auto val = settings.value(group->objectName(), "").toString();\
		foreach(auto rbt, group->buttons()) {	\
			if(rbt->text() == val)	{			\
				rbt->setChecked(true);			\
				break;							\
			}									\
		}										\
		*diconnector << CONNECT(group, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), [=](QAbstractButton *button) { \
			QSettings localSettings(EXPERIMENT_VALUES_INI, QSettings::IniFormat); \
			localSettings.beginGroup(GetFullName()); \
			localSettings.setValue(group->objectName(), button->text()); \
		});										\
	}

#define _INSERT_RADIO_BUTTON_LAYOUT(text)	\
	{											\
		auto button = RBT(text);				\
		OBJ_PROP(button, "experiment-params-widget", "low-margin"); \
		group->addButton(button);				\
		if(0 == group->checkedButton()) {		\
			button->setChecked(true);			\
		}										\
		butLay->addWidget(button);				\
	}

#define _START_DROP_DOWN(obj_name, row, col)			\
	{													\
		auto combo = OBJ_NAME(CMB(), obj_name);			\
		OBJ_PROP(combo, "experiment-params-widget", "low-margin"); \
		QListView *comboList = OBJ_NAME(new QListView, "combo-list"); \
		combo->setView(comboList);						\
		lay->addWidget(combo, row, col);

#define _END_DROP_DOWN()								\
		auto val = settings.value(combo->objectName(), "").toString();\
		for(int i = 0; i < combo->count(); ++i) {		\
			if (val == combo->itemText(i)) {			\
				combo->setCurrentIndex(i);				\
				break;									\
			}											\
		}												\
		*diconnector << CONNECT(combo, &QComboBox::currentTextChanged, [=](const QString &str) { \
			QSettings localSettings(EXPERIMENT_VALUES_INI, QSettings::IniFormat); \
			localSettings.beginGroup(GetFullName()); \
			localSettings.setValue(combo->objectName(), str); \
		});												\
	}

#define _ADD_DROP_DOWN_ITEM(text)	\
		combo->addItem(text);

#define _INSERT_VERTICAL_SPACING(row)					\
	lay->addWidget(OBJ_PROP(WDG(), "experiment-params-widget", "vertical-spacing"), row, 0, 1, -1);

#define _SET_ROW_STRETCH(row, stretch)	\
	lay->setRowStretch(row, stretch);

#define _SET_COL_STRETCH(col, stretch)	\
	lay->setColumnStretch(col, stretch);

#define USER_INPUT_START(name)	\
	auto *ret = WDG();		\
	OBJ_NAME(ret, name);	\
	auto *lay = NO_SPACING(NO_MARGIN(new QGridLayout(ret))); \
	QSettings settings(EXPERIMENT_VALUES_INI, QSettings::IniFormat); \
	settings.beginGroup(GetFullName()); \
	auto diconnector = new Disconnector(ret);

#define USER_INPUT_END()	return ret;


#define NODES_DATA_START(widget, name)	\
	NodesData ret;						\
	if (widget->objectName() != name) { \
		return ret;						\
	}									\
	ExperimentNode_t exp;				\
	memset(&exp, 0x00, sizeof(ExperimentNode_t));

#define NODES_DATA_END()	return ret;


#define PUSH_NEW_NODE_DATA()		\
	ret << QByteArray((char*)&exp, sizeof(ExperimentNode_t)); \
	memset(&exp, 0x00, sizeof(ExperimentNode_t));


#define GET_TEXT_INPUT_VALUE(var, obj_name)					\
	auto var ## Wdg = wdg->findChild<QLineEdit*>(obj_name);	\
	if(0 == var ## Wdg) {	\
		return ret;			\
	}						\
	var = var ## Wdg->text().toLongLong();

#define GET_TEXT_INPUT_VALUE_DOUBLE(var, obj_name)					\
	auto var ## Wdg = wdg->findChild<QLineEdit*>(obj_name);	\
	if(0 == var ## Wdg) {	\
		return ret;			\
	}						\
	var = var ## Wdg->text().toDouble();

#define GET_SELECTED_RADIO(var, obj_name)						\
	auto var ## Grp = wdg->findChild<QButtonGroup*>(obj_name);	\
	if(0 == var ## Grp) {	\
		return ret;			\
	}						\
	var = "";				\
	auto var ## Checked = var ## Grp->checkedButton();	\
	if (var ## Checked) {								\
		var = var ## Checked->text();					\
	}

#define GET_SELECTED_DROP_DOWN(var, obj_name)			\
	auto var ## DD = wdg->findChild<QComboBox*>(obj_name);	\
	if(0 == var ## DD) {				\
		return ret;						\
	}									\
	var = var ## DD->currentText();
