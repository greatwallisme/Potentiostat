#pragma once

#include <QString>
#include <QPixmap>
#include <QButtonGroup>
#include <QListView>

#include <QWidget>

#include <QHBoxLayout>

#include <UIHelper.hpp>

#define _INSERT_LEFT_COMMENT(text, row, col) \
	lay->addWidget(OBJ_PROP(OBJ_NAME(LBL(text), "experiment-params-comment"), "comment-placement", "left"),	row, col);

#define _INSERT_RIGHT_COMMENT(text, row, col) \
	lay->addWidget(OBJ_PROP(OBJ_NAME(LBL(text), "experiment-params-comment"), "comment-placement", "right"), row, col);

#define _INSERT_TEXT_INPUT(default_value, obj_name, row, col) \
	lay->addWidget(OBJ_NAME(new QLineEdit(QString("%1").arg(default_value)), obj_name), row, col);

#define _START_RADIO_BUTTON_GROUP(obj_name)		\
	{										\
		auto group = new QButtonGroup(ret);	\
		OBJ_NAME(group, obj_name);

#define _END_RADIO_BUTTON_GROUP() \
	}

#define _START_RADIO_BUTTON_GROUP_HORIZONTAL_LAYOUT(obj_name, row, col)	\
	_START_RADIO_BUTTON_GROUP(obj_name);						\
	auto butLay = NO_SPACING(NO_MARGIN(new QHBoxLayout));		\
	lay->addLayout(butLay, row, col);

#define _END_RADIO_BUTTON_GROUP_LAYOUT() \
		butLay->addStretch(1);			 \
	}

#define _INSERT_RADIO_BUTTON_LAYOUT(text)	\
	{											\
		auto button = RBT(text);				\
		group->addButton(button);				\
		if(0 == group->checkedButton()) {		\
			button->setChecked(true);			\
		}										\
		butLay->addWidget(button);				\
	}


#define _INSERT_RADIO_BUTTON(text, row, col)	\
	{											\
		auto button = RBT(text);				\
		group->addButton(button);				\
		if(0 == group->checkedButton()) {		\
			button->setChecked(true);			\
		}										\
		lay->addWidget(button, row, col);		\
	}

#define _START_DROP_DOWN(obj_name, row, col)			\
	{													\
		auto combo = OBJ_NAME(CMB(), obj_name);			\
		QListView *comboList = OBJ_NAME(new QListView, "combo-list"); \
		combo->setView(comboList);						\
		lay->addWidget(combo, row, col);

#define _END_DROP_DOWN()								\
	}

#define _ADD_DROP_DOWN_ITEM(text)	\
		combo->addItem(text);

#define _SET_ROW_STRETCH(row, stretch)	\
	lay->setRowStretch(row, stretch);

#define _SET_COL_STRETCH(col, stretch)	\
	lay->setColumnStretch(col, stretch);

#define USER_INPUT_START(name)	\
	auto *ret = WDG();		\
	OBJ_NAME(ret, name);	\
	auto *lay = NO_SPACING(NO_MARGIN(new QGridLayout(ret)));

#define USER_INPUT_END()	return ret;


#define NODES_DATA_START(widget, name)	\
	QByteArray ret;						\
	if (widget->objectName() != name) { \
		return ret;						\
	}

#define NODES_DATA_END()	return ret;


#define PUSH_NEW_NODE_DATA()		\
	ret += QByteArray((char*)&exp, sizeof(ExperimentNode_t));


#define GET_TEXT_INPUT_VALUE(var, obj_name)					\
	auto var ## Wdg = wdg->findChild<QLineEdit*>(obj_name);	\
	if(0 == var ## Wdg) {	\
		return ret;			\
	}						\
	var = var ## Wdg->text().toLongLong();

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
	