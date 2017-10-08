#include <HwListButton.h>

#include <UIHelper.hpp>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QModelIndex>

#include <QApplication>

QMap<HwListButton::State, QString> colorStates{
	{ HwListButton::ACTIVE, QString("active") },
	{ HwListButton::STOPPED, QString("stopped") },
	{ HwListButton::MIDDLE, QString("middle") }
};

HwListButton::HwListButton(const QModelIndex &_index, const QString &text) :
	index(_index)
{
	auto lay = NO_SPACING(NO_MARGIN(new QHBoxLayout(this)));

	QLabel *lbl;

	lay->addWidget(pbt = new QPushButton);
	lay->addWidget(lbl = new QLabel(text));

	connect(pbt, &QPushButton::clicked, [this]() {
		this->Clicked(this->index);
	});

	SetColorState(HwListButton::STOPPED);
}
void HwListButton::SetColorState(HwListButton::State state) {
	OBJ_PROP(pbt, "color-state", colorStates.value(state, "middle"));
	pbt->setStyleSheet(qobject_cast<QApplication *>(QApplication::instance())->styleSheet());
}