#pragma once

#include <QString>
#include <QPixmap>

#include <QWidget>

#include <UIHelper.hpp>

#define _INSERT_LEFT_COMMENT(text, row, col) \
	lay->addWidget(OBJ_PROP(OBJ_NAME(LBL(text), "experiment-params-comment"), "comment-placement", "left"),	row, col);

#define _INSERT_RIGHT_COMMENT(text, row, col) \
	lay->addWidget(OBJ_PROP(OBJ_NAME(LBL(text), "experiment-params-comment"), "comment-placement", "right"), row, col);

#define _INSERT_TEXT_INPUT(default_value, obj_name, row, col) \
	lay->addWidget(OBJ_NAME(new QLineEdit(QString("%1").arg(default_value)), obj_name), row, col);

#define _SET_ROW_STRETCH(row, stretch)	\
	lay->setRowStretch(row, stretch);

#define _SET_COL_STRETCH(col, stretch)	\
	lay->setColumnStretch(col, stretch);

#define USER_INPUT_START(name)	\
	auto *ret = WDG();		\
	OBJ_NAME(ret, name);	\
	auto *lay = NO_SPACING(NO_MARGIN(new QGridLayout(ret)));

#define USER_INPUT_END()	return ret;


#define GET_TEXT_INPUT_VALUE(var, obj_name)					\
	auto var ## Wdg = wdg->findChild<QLineEdit*>(obj_name);	\
	if(0 == var ## Wdg) {	\
		return ret;			\
	}						\
	var = var ## Wdg->text().toLongLong();