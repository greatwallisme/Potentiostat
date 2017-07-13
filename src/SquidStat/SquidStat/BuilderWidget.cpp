#include "BuilderWidget.h"

#include "Log.h"
#include "UIHelper.hpp"

#include <QSpinBox>
#include <QPainter>
#include <QEvent>

BuilderContainer::BuilderContainer(qint32 rep, Type t) :
	repetition(rep),
	type(t),
	w(0) 
{
}
QList<QLine> GetLines(QWidget *w, const LineDirection &ld) {
	QList<QLine> ret;

	QRect rect = w->rect();
	QMargins margins = w->contentsMargins();
	QPoint pos = w->pos();

	if (ld.testFlag(LD_LEFT)) {
		QPoint startPoint;
		startPoint.setX(pos.x() - margins.left());
		startPoint.setY(pos.y() + rect.height() / 2);

		QPoint endPoint;
		endPoint.setX(pos.x() + margins.left());
		endPoint.setY(pos.y() + rect.height() / 2);

		ret << QLine(startPoint, endPoint);
	}
	if (ld.testFlag(LD_RIGHT)) {
		QPoint startPoint;
		startPoint.setX(pos.x() + rect.width() - margins.right());
		startPoint.setY(pos.y() + rect.height() / 2);

		QPoint endPoint;
		endPoint.setX(pos.x() + rect.width() + margins.right());
		endPoint.setY(pos.y() + rect.height() / 2);

		ret << QLine(startPoint, endPoint);
	}
	if (ld.testFlag(LD_TOP)) {
		QPoint startPoint;
		startPoint.setX(pos.x() + rect.width() / 2);
		startPoint.setY(pos.y() - margins.top());

		QPoint endPoint;
		endPoint.setX(pos.x() + rect.width() / 2);
		endPoint.setY(pos.y() + margins.top());

		ret << QLine(startPoint, endPoint);
	}
	if (ld.testFlag(LD_BOTTOM)) {
		QPoint startPoint;
		startPoint.setX(pos.x() + rect.width() / 2);
		startPoint.setY(pos.y() + rect.height() - margins.bottom());

		QPoint endPoint;
		endPoint.setX(pos.x() + rect.width() / 2);
		endPoint.setY(pos.y() + rect.height() + margins.bottom());

		ret << QLine(startPoint, endPoint);
	}

	return ret;
}


BuildExpContainer::BuildExpContainer(QWidget *parent, const BuilderContainer &cont) :
	QFrame(parent),
	bc(cont)
{
	OBJ_NAME(this, "node-builder-container");

	auto containerWdgLay = NO_MARGIN(NO_SPACING(new QVBoxLayout(this)));
	auto elementsLay = NO_MARGIN(new QVBoxLayout);
	elementsLay->setSpacing(2);

	for (auto it = bc.elements.begin(); it != bc.elements.end(); ++it) {
		if (!it->w) {
			continue;
		}

		elementsLay->addWidget(it->w);
	}
	QSpinBox *mult;
	containerWdgLay->addLayout(elementsLay);
	containerWdgLay->addWidget(mult = OBJ_NAME(new QSpinBox, "node-builder-multiplier-container"));

	mult->setMinimum(1);
	mult->setMaximum(99999);
	mult->setValue(bc.repetition);
}
void BuildExpContainer::paintEvent(QPaintEvent *e) {
	for (auto it = bc.elements.begin(); it != bc.elements.end(); ++it) {
		if (it->type == BuilderContainer::SET) {
			continue;
		}

		QList<QLine> lines = GetLines(it->w, it->ld);

		QPainter painter(this);
		painter.setPen(QPen(QColor("#80939a"), 2));
		foreach(auto &line, lines) {
			painter.drawLine(line);
		}
	}
}





BuilderWidget::BuilderWidget(QWidget *parent) :
	QFrame(parent)
{
	OBJ_NAME(this, "build-exp-holder");

	ui.elementsLay = 0;

	InitContainer();
	UpdateLines();
	PlaceWidgets();
}
void BuilderWidget::InitContainer() {
	container.repetition = 20;
	container.elements
		<< BuilderContainer(1)
		<< BuilderContainer(2)
		<< BuilderContainer(10, BuilderContainer::SET)
		<< BuilderContainer(15, BuilderContainer::SET)
		<< BuilderContainer(8)
		<< BuilderContainer(9)
		<< BuilderContainer(10);

	container.elements[2].elements
		<< BuilderContainer(3)
		<< BuilderContainer(4)
		<< BuilderContainer(5);

	container.elements[3].elements
		<< BuilderContainer(6)
		<< BuilderContainer(7);
}
void BuilderWidget::PlaceWidgets() {
	if (!ui.elementsLay) {
		ui.elementsLay = NO_MARGIN(new QGridLayout(this));
		ui.elementsLay->setSpacing(2);
	}

	int column = 0;
	int maxRowSpan = 0;
	foreach(auto &elem, container.elements) {
		int rowSpan = (elem.type == BuilderContainer::ELEMENT) ? 1 : (elem.elements.size() + 1);
		if (rowSpan > maxRowSpan) {
			maxRowSpan = rowSpan;
		}

		ui.elementsLay->addWidget(elem.w, 0, column++, rowSpan, 1);
	}
	ui.elementsLay->setRowStretch(maxRowSpan, 1);
	ui.elementsLay->setColumnStretch(column, 1);
	
	this->update();
}
void BuilderWidget::UpdateLines() {

	for (auto it = container.elements.begin(); it != container.elements.end(); ++it) {
		it->ld = LD_NONE;
		if (it != container.elements.begin()) {
			it->ld |= LD_LEFT;
		}
		if (it != (--container.elements.end())) {
			it->ld |= LD_RIGHT;
		}

		switch (it->type) {
		case BuilderContainer::ELEMENT:
			if (!it->w) {
				it->w = CreateBuildExpElementWidget(*it);
				//it->w->installEventFilter(new ElementEventFiler(it->w, it->ld));
			}
			break;

		case BuilderContainer::SET:
			for (auto cIt = it->elements.begin(); cIt != it->elements.end(); ++cIt) {
				cIt->ld = LD_NONE;
				if (cIt == it->elements.begin()) {
					cIt->ld = it->ld;
				}
				if (cIt != it->elements.begin()) {
					cIt->ld |= LD_TOP;
				}
				if (cIt != (--it->elements.end())) {
					cIt->ld |= LD_BOTTOM;
				}

				if (!cIt->w) {
					cIt->w = CreateBuildExpElementWidget(*cIt);
					//cIt->w->installEventFilter(new ElementEventFiler(cIt->w, cIt->ld));
				}
			}
			if (!it->w) {
				it->w = CreateBuildExpContainerWidget(*it);
			}
			break;
		}
	}
}
QWidget* BuilderWidget::CreateBuildExpElementWidget(const BuilderContainer &bc) {
	if (bc.type != BuilderContainer::ELEMENT) {
		LOG() << "It is not an ELEMENT!";
		return 0;
	}

	auto *w = OBJ_NAME(new QFrame(), "node-builder-owner");

	auto lay = NO_SPACING(NO_MARGIN(new QVBoxLayout(w)));

	QSpinBox *mult;
	QLabel *label;

	lay->addWidget(label = OBJ_NAME(new QLabel, "node-builder-label"));
	lay->addWidget(mult = OBJ_NAME(new QSpinBox(), "node-builder-multiplier-single"));

	label->setPixmap(QPixmap(":/GUI/Resources/node-pic-example.png"));

	mult->setMinimum(1);
	mult->setMaximum(99999);
	mult->setValue(bc.repetition);

	return w;
}
QWidget* BuilderWidget::CreateBuildExpContainerWidget(const BuilderContainer &bc) {
	if (bc.type != BuilderContainer::SET) {
		LOG() << "It is not a SET!";
		return 0;
	}

	return new BuildExpContainer(this, bc);
}
void BuilderWidget::paintEvent(QPaintEvent *e) {
	QList<QLine> lines;
	for (auto it = container.elements.begin(); it != container.elements.end(); ++it) {
		if (it->type == BuilderContainer::SET) {
			if (!it->elements.size()) {
				continue;
			}
			auto fIt = it->elements.begin();
			auto tempLines = GetLines(fIt->w, fIt->ld);

			for (auto lineIt = tempLines.begin(); lineIt != tempLines.end(); ++lineIt) {
				lineIt->setP1(lineIt->p1() + it->w->pos());
				lineIt->setP2(lineIt->p2() + it->w->pos());
			}
			lines << tempLines;

			continue;
		}

		lines << GetLines(it->w, it->ld);
	}

	QPainter painter(this);
	painter.setPen(QPen(QColor("#80939a"), 2));
	foreach(auto &line, lines) {
		painter.drawLine(line);
	}
}