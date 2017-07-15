#include "BuilderWidget.h"

#include "Log.h"
#include "UIHelper.hpp"

#include <QSpinBox>
#include <QPainter>
#include <QEvent>

#include <QMimeData>

#include <QEventLoop>

#define BUILDER_ELEMENT_SPACING		2

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

	ui.elementsLay = 0;
	ui.containerWdgLay = NO_MARGIN(NO_SPACING(new QVBoxLayout(this)));

	QSpinBox *mult;
	ui.containerWdgLay->insertWidget(1, mult = OBJ_NAME(new QSpinBox, "node-builder-multiplier-container"));

	mult->setMinimum(1);
	mult->setMaximum(99999);
	mult->setValue(bc.repetition);
}
void BuildExpContainer::PlaceWidgets() {
	if (!ui.elementsLay) {
		ui.elementsLay = NO_MARGIN(new QVBoxLayout);
		ui.elementsLay->setSpacing(BUILDER_ELEMENT_SPACING);
	
		ui.containerWdgLay->insertLayout(0, ui.elementsLay);
	}
	while (ui.elementsLay->count()) {
		ui.elementsLay->removeItem(ui.elementsLay->itemAt(0));
	}

	for (int i = 0; i < bc.elements.count(); ++i) {
		auto cur = bc.elements.at(i);
		if (!cur.w) {
			continue;
		}

		ui.elementsLay->insertWidget(i, cur.w);
	}
	
	this->update();
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
void BuildExpContainer::resizeEvent(QResizeEvent *e) {
	emit UpdateBackgroundMap();
}

BuilderWidget::BuilderWidget(QWidget *parent) :
	QFrame(parent)
{
	background.currentArea = 0;

	connect(this, &BuilderWidget::EnqueueUpdateBackgroundMap,
		this, &BuilderWidget::UpdateBackgroundMap, Qt::QueuedConnection);

	OBJ_NAME(this, "build-exp-holder");
	this->setAcceptDrops(true);

	ui.elementsLay = 0;

	InitContainer();
	InitWidgets();
	UpdateLines();
	PlaceWidgets();
	emit EnqueueUpdateBackgroundMap();
}
void BuilderWidget::InitContainer() {
	/*/
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
	//*/
}
void BuilderWidget::PlaceWidgets() {
	if (!ui.elementsLay) {
		ui.elementsLay = NO_MARGIN(new QGridLayout(this));
		ui.elementsLay->setSpacing(BUILDER_ELEMENT_SPACING);
	}
	while (ui.elementsLay->count()) {
		ui.elementsLay->removeItem(ui.elementsLay->itemAt(0));
	}

	int rows = ui.elementsLay->rowCount();
	for (int i = 0; i < rows; ++i) {
		ui.elementsLay->setRowStretch(i, 0);
	}

	int cols = ui.elementsLay->columnCount();
	for (int i = 0; i < cols; ++i) {
		ui.elementsLay->setColumnStretch(i, 0);
	}

	int column = 0;
	int maxRowSpan = 0;
	for (auto it = container.elements.begin(); it != container.elements.end(); ++it) {
		int rowSpan = (it->type == BuilderContainer::ELEMENT) ? 1 : (it->elements.size() + 1);
		if (rowSpan > maxRowSpan) {
			maxRowSpan = rowSpan;
		}

		ui.elementsLay->addWidget(it->w, 0, column++, rowSpan, 1);
		if (it->w->isHidden()) {
			it->w->show();
		}
	}
	ui.elementsLay->setRowStretch(maxRowSpan, 1);
	ui.elementsLay->setColumnStretch(column, 1);
	
	emit RequestPlaceWidgets();

	this->update();
}
void BuilderWidget::InitWidgets() {
	for (auto it = container.elements.begin(); it != container.elements.end(); ++it) {
		switch (it->type) {
			case BuilderContainer::ELEMENT:
				if (!it->w) {
					it->w = CreateBuildExpElementWidget(*it);
					it->w->hide();
				}
				break;

			case BuilderContainer::SET:
				for (auto cIt = it->elements.begin(); cIt != it->elements.end(); ++cIt) {
					if (!cIt->w) {
						cIt->w = CreateBuildExpElementWidget(*cIt);
					}
				}
				if (!it->w) {
					it->w = CreateBuildExpContainerWidget(*it);
				}
				break;
		}
	}
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

	auto *w = OBJ_NAME(new QFrame(this), "node-builder-owner");

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

	auto cont = new BuildExpContainer(this, bc);

	connect(cont, &BuildExpContainer::UpdateBackgroundMap,
		this, &BuilderWidget::UpdateBackgroundMap, Qt::QueuedConnection);
	connect(this, &BuilderWidget::RequestPlaceWidgets,
		cont, &BuildExpContainer::PlaceWidgets);

	return cont;
}
void BuilderWidget::paintEvent(QPaintEvent *e) {
	QPainter painter(this);
	
	QList<QLine> lines;
	//*
	for (auto it = container.elements.begin(); it != container.elements.end(); ++it) {
		if (it->type == BuilderContainer::SET) {
			auto rect = it->w->rect();
			auto margins = it->w->contentsMargins();
			auto pos = it->w->pos();
			painter.fillRect(QRect(pos + QPoint(margins.left(), margins.top()),
				pos + QPoint(rect.width(), rect.height()) - QPoint(margins.right() + 1, margins.bottom())),
				QColor("#c3dff5"));

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
	/*/
	painter.drawImage(0, 0, background.map);
	//*/

	painter.setPen(QPen(QColor("#80939a"), 2));
	foreach(auto &line, lines) {
		painter.drawLine(line);
	}

	if (background.currentArea) {
		painter.setPen(QPen(QColor("#7b1a1e"), 2));
		painter.drawLine(background.currentArea->dropLine);
	}
}
void BuilderWidget::dragEnterEvent(QDragEnterEvent *e) {
	if (e->mimeData()->hasFormat(ELEMENT_MIME_TYPE)) {
		e->accept();
	}
	else {
		e->ignore();
	}
}
void BuilderWidget::dragLeaveEvent(QDragLeaveEvent *e) {
	background.currentArea = 0;
	this->update();
}
void BuilderWidget::dragMoveEvent(QDragMoveEvent *e) {
	do {
		if (!e->mimeData()->hasFormat(ELEMENT_MIME_TYPE)) {
			break;
		}
		auto pos = e->pos();

		auto color = background.map.pixelColor(pos);
		
		if (!background.areas.contains(color)) {
			break;
		}

		if (BackgroundDescriptor::IGNORE == background.areas[color].dragMoveAction) {
			break;
		}

		background.currentArea = &background.areas[color];
		e->accept();
		this->update();

		return;
	} while (0);

	background.currentArea = 0;
	e->ignore();
	this->update();
}
void BuilderWidget::dropEvent(QDropEvent *e) {
	if (e->mimeData()->hasFormat(ELEMENT_MIME_TYPE)) {
		if (!background.currentArea) {
			return;
		}

		auto area = background.currentArea;
		
		static int i = 100;
		if (area->list) {
			BuilderContainer bc;
			bc.type = BuilderContainer::ELEMENT;
			bc.repetition = i++;
			bc.w = 0;

			area->list->insert(area->before, bc);
		}
		else {
			BuilderContainer bc;
			bc.type = BuilderContainer::ELEMENT;
			bc.repetition = i++;
			bc.w = 0;

			BuilderContainer oldBc = *area->before;

			BuilderContainer newCont;
			newCont.type = BuilderContainer::SET;
			newCont.repetition = 1;
			newCont.w = 0;
			
			if (area->dropAction == BackgroundDescriptor::BEFORE) {
				newCont.elements << bc;
			}

			newCont.elements << oldBc;

			if (area->dropAction == BackgroundDescriptor::AFTER) {
				newCont.elements << bc;
			}

			*area->before = newCont;
		}

		InitWidgets();
		UpdateLines();
		PlaceWidgets();
		emit EnqueueUpdateBackgroundMap();
	}

	background.currentArea = 0;
	this->update();
}
void BuilderWidget::resizeEvent(QResizeEvent *e) {
	emit EnqueueUpdateBackgroundMap();
}

QColor GetColor(quint32 id) {
	QString colorName = QString("#%1").arg(0x00ffffff & id, 6, 0x10, QChar('0')).toUpper();
	return QColor(colorName);
}
quint32 GetNexColorId(quint32 id) {
	++id;
	//id *= 123;
	return id;
}

bool operator < (const QColor &a, const QColor &b) {
	return a.name() < b.name();
}
void BuilderWidget::UpdateBackgroundMap() {
	background.map = QImage(this->size(), QImage::Format_RGB32);
	background.areas.clear();

	quint32 colorId = 0;

	QPainter painter(&background.map);

	auto color = GetColor(colorId++);
	colorId = GetNexColorId(colorId);
	background.areas[color].dragMoveAction = BackgroundDescriptor::IGNORE;

	painter.fillRect(background.map.rect(), color);

	QPoint startPos(0, this->contentsMargins().top());
	QPoint endPos;

	QRect rect;
	QPoint pos;
	QMargins margins;

	if (!container.elements.count()) {
		color = GetColor(colorId++);
		background.areas[color].dragMoveAction = BackgroundDescriptor::ACCEPT;
		background.areas[color].dropLine = QLine(-1, -1, -2, -2);
		background.areas[color].before = container.elements.end();
		background.areas[color].list = &container.elements;
		
		painter.fillRect(background.map.rect(), color);

		this->update();
		return;
	}

	{
		auto it = container.elements.begin();

		color = GetColor(colorId++);
		colorId = GetNexColorId(colorId);
		background.areas[color].dragMoveAction = BackgroundDescriptor::ACCEPT;

		pos = it->w->pos();

		if (it->type == BuilderContainer::SET) {
			rect = it->elements.begin()->w->rect();
			margins = it->elements.begin()->w->contentsMargins();
		}
		else {
			rect = it->w->rect();
			margins = it->w->contentsMargins();
		}

		background.areas[color].dropLine = QLine(pos + QPoint(-1, 0),
			pos + QPoint(-1, rect.height()));
		background.areas[color].before = it;
		background.areas[color].list = &container.elements;

		endPos = pos + QPoint(rect.width() / 2, rect.height());

		painter.fillRect(startPos.x(), startPos.y(),
			endPos.x() - startPos.x(), endPos.y() - startPos.y(), color);
	}

	for (auto it = container.elements.begin(); it != container.elements.end(); ++it) {
		color = GetColor(colorId++);
		colorId = GetNexColorId(colorId);
		background.areas[color].dragMoveAction = BackgroundDescriptor::ACCEPT;
		
		pos = it->w->pos();

		if (it->type == BuilderContainer::SET) {
			rect = it->elements.begin()->w->rect();
			margins = it->elements.begin()->w->contentsMargins();
		}
		else {
			rect = it->w->rect();
			margins = it->w->contentsMargins();
		}

		// Right area
		background.areas[color].dropLine =
			QLine(pos + QPoint(rect.width() + 1, 0), pos + QPoint(rect.width() + 1, rect.height()));
		background.areas[color].before = ++it; --it;
		background.areas[color].list = &container.elements;

		startPos = pos + QPoint(rect.width() / 2 - BUILDER_ELEMENT_SPACING, 0);
		endPos = pos + QPoint(rect.width() * 3 / 2, rect.height());

		painter.fillRect(startPos.x(), startPos.y(),
			endPos.x() - startPos.x(), endPos.y() - startPos.y(), color);

		// Top area
		color = GetColor(colorId++);
		colorId = GetNexColorId(colorId);
		background.areas[color].dragMoveAction = BackgroundDescriptor::ACCEPT;

		background.areas[color].dropLine =
			QLine(pos + QPoint(1, -1),
				pos + QPoint(rect.width() - 1, -1));
		if (it->type == BuilderContainer::SET) {
			background.areas[color].before = it->elements.begin();
			background.areas[color].list = &it->elements;
		}
		else {
			background.areas[color].before = it;
			background.areas[color].list = 0;
			background.areas[color].dropAction = BackgroundDescriptor::BEFORE;
		}

		startPos = pos + QPoint(margins.left() + 1, -margins.top()*2);
		endPos = pos + QPoint(rect.width() - margins.right() - 1, margins.top() * 2);

		painter.fillRect(startPos.x(), startPos.y(),
			endPos.x() - startPos.x(), endPos.y() - startPos.y(), color);

		// Bottom area
		color = GetColor(colorId++);
		colorId = GetNexColorId(colorId);
		background.areas[color].dragMoveAction = BackgroundDescriptor::ACCEPT;

		background.areas[color].dropLine =
			QLine(pos + QPoint(1, rect.height()-1),
				pos + QPoint(rect.width() - 1, rect.height()-1));

		if (it->type == BuilderContainer::SET) {
			background.areas[color].before = ++it->elements.begin();
			background.areas[color].list = &it->elements;
		}
		else {
			background.areas[color].before = it;
			background.areas[color].list = 0;
			background.areas[color].dropAction = BackgroundDescriptor::AFTER;
		}

		startPos = pos + QPoint(margins.left(), rect.height() - margins.bottom() * 2);
		endPos = pos + QPoint(rect.width() - margins.right(), rect.height() + margins.bottom() * 2);

		painter.fillRect(startPos.x(), startPos.y(),
			endPos.x() - startPos.x(), endPos.y() - startPos.y(), color);
	}

	for (auto it = container.elements.begin(); it != container.elements.end(); ++it) {
		if (it->type != BuilderContainer::SET) {
			continue;
		}

		auto cIt = it->elements.begin();
		if (cIt == it->elements.end()) {
			continue;
		}

		++cIt;

		QPoint contPos = it->w->pos();

		for (; cIt != it->elements.end(); ++cIt) {
			pos = cIt->w->pos();

			rect = cIt->w->rect();
			margins = cIt->w->contentsMargins();

			// Bottom area
			color = GetColor(colorId++);
			colorId = GetNexColorId(colorId);
			background.areas[color].dragMoveAction = BackgroundDescriptor::ACCEPT;

			background.areas[color].dropLine =
				QLine(contPos + pos + QPoint(1, rect.height() - 1),
					contPos + pos + QPoint(rect.width() - 1, rect.height() - 1));
			background.areas[color].before = ++cIt; --cIt;
			background.areas[color].list = &it->elements;

			startPos = contPos + pos + QPoint(margins.left(), rect.height() - margins.bottom() * 2);
			endPos = contPos + pos + QPoint(rect.width() - margins.right(), rect.height() + margins.bottom() * 2);

			painter.fillRect(startPos.x(), startPos.y(),
				endPos.x() - startPos.x(), endPos.y() - startPos.y(), color);
		}
	}

	this->update();
}