#include "BuilderWidget.h"

#include "Log.h"
#include "UIHelper.hpp"
#include "Disconnector.h"

#include <QSpinBox>
#include <QPainter>
#include <QEvent>

#include <QMimeData>

#include <QEventLoop>
#include <QtMath>
#include <QDrag>

#define BUILDER_ELEMENT_SPACING		2

BuilderContainer::BuilderContainer(qint32 rep, Type t) :
	repeats(rep),
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

class ElementEventFilter : public QObject {
public:
	ElementEventFilter(QObject *parent, BuilderWidget *bw) :
		QObject(parent), _bw(bw), pressed(false), dragged(false) {}

	bool eventFilter(QObject *obj, QEvent *e) {
		bool ret = false;

		switch (e->type()) {
			case QEvent::MouseButtonPress:
				if (!IsIgnoreArea(obj, e)) {
					pressed = true;
					dragged = false;
					pressButton = ((QMouseEvent*)e)->button();
					pressPoint = ((QMouseEvent*)e)->pos();

					ret = true;
				}

				break;

			case QEvent::MouseMove:
				if(pressed && !dragged) {
					QPoint pos = ((QMouseEvent*)e)->pos();
					QLine moveVector(pressPoint, pos);
					auto length = qSqrt(moveVector.dx() * moveVector.dx() + moveVector.dy() * moveVector.dy());
					if (length > 3) {
						dragged = true;
						ret = ProcessDrag(obj, e);
					}
				}
				break;

			case QEvent::MouseButtonRelease:
				if (pressed && !dragged) {
					ret = ProcessSelection(obj, e);
				}
				pressed = false;
				dragged = false;
				break;
		}

		return ret;
	}

private:
	bool IsIgnoreArea(QObject *obj, QEvent *e) {
		auto w = qobject_cast<QWidget*>(obj);
		if (!w) {
			return true;
		}

		auto me = (QMouseEvent*)e;

		auto marg = w->contentsMargins();
		QPoint bottomRight = QPoint(w->width(), w->height());
		bottomRight -= QPoint(marg.right() - 3, marg.bottom() - 3);

		QPoint topLeft = QPoint(marg.left() - 3, marg.top() - 3);

		if (!QRect(topLeft, bottomRight).contains(me->pos())) {
			return true;
		}

		return false;
	}
	bool ProcessSelection(QObject *obj, QEvent *e) {
		auto w = qobject_cast<QWidget*>(obj);
		if (!w) {
			return false;
		}

		if (IsIgnoreArea(obj, e)) {
			return false;
		}

		emit _bw->ElementSelected(w);
		return true;
	}
	bool ProcessDrag(QObject *obj, QEvent *e) {
		QWidget *w = qobject_cast<QWidget*>(obj);
		if (!w) {
			return false;
		}

		QMouseEvent *me = (QMouseEvent*)e;

		if (IsIgnoreArea(obj, e)) {
			return false;
		}

		if (pressButton == Qt::LeftButton) {
			auto margins = w->contentsMargins();
			auto rect = w->rect();

			QPoint startPoint;
			startPoint.setX(margins.left() - 1);
			startPoint.setY(margins.top() - 1);
			QPoint endPoint;
			endPoint.setX(rect.width() - margins.right() + 1);
			endPoint.setY(rect.height() - margins.bottom());

			QUuid id = _bw->GetId(w);

			auto mime = new QMimeData;
			mime->setData(CONTAINER_MIME_TYPE, id.toByteArray());

			auto pixmap = w->grab(QRect(startPoint, endPoint));
			pixmap = pixmap.scaledToHeight((endPoint.y() - startPoint.y()) / 2, Qt::SmoothTransformation);

			QDrag *drag = new QDrag(obj);
			drag->setMimeData(mime);
			drag->setPixmap(pixmap);
			drag->setHotSpot((pressPoint - QPoint(margins.left(), margins.top())) / 2);

			Qt::DropAction dropAction = drag->exec(Qt::MoveAction, Qt::MoveAction);
			return true;
		}

		return false;
	}

	QPoint pressPoint;
	Qt::MouseButton pressButton;
	bool pressed;
	bool dragged;

	BuilderWidget *_bw;
};

BuildExpContainer::BuildExpContainer(BuilderWidget *parent, const BuilderContainer &cont, QUuid id) :
	QFrame(parent),
	_bw(parent),
	bc(cont),
	pressed(false),
	dragged(false)
{
	OBJ_NAME(this, "node-builder-container");

	ui.elementsLay = 0;
	ui.containerWdgLay = NO_MARGIN(NO_SPACING(new QVBoxLayout(this)));

	QSpinBox *mult;
	ui.containerWdgLay->insertWidget(1, mult = OBJ_NAME(new QSpinBox, "node-builder-multiplier-container"));

	mult->setMinimum(1);
	mult->setMaximum(99999);
	mult->setValue(bc.repeats);

	auto disconnector = new Disconnector(mult);

	*disconnector <<
	connect(mult, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int val) {
		_bw->SetRepeats(id, val);
	});
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
void BuildExpContainer::mousePressEvent(QMouseEvent *me) {
	if (!IsIgnoreArea(me)) {
		pressed = true;
		dragged = false;
		pressButton = me->button();
		pressPoint = me->pos();

		me->accept();
	}
	else {
		me->ignore();
	}
}
void BuildExpContainer::mouseMoveEvent(QMouseEvent *me) {
	if (pressed && !dragged) {
		QPoint pos = me->pos();
		QLine moveVector(pressPoint, pos);

		auto length = qSqrt(moveVector.dx() * moveVector.dx() + moveVector.dy() * moveVector.dy());
		
		if (length > 3) {
			dragged = true;
			if (ProcessDrag(me)) {
				me->accept();
			}
			else {
				me->ignore();
			}
		}
	}
	else {
		me->ignore();
	}
}
void BuildExpContainer::mouseReleaseEvent(QMouseEvent *me) {
	if (pressed && !dragged) {
		if (ProcessSelection(me)) {
			me->accept();
		}
		else {
			me->ignore();
		}
	}
	else {
		me->ignore();
	}

	pressed = false;
	dragged = false;
}
bool BuildExpContainer::IsIgnoreArea(QMouseEvent *me) {
	auto w = this;
	
	auto marg = w->contentsMargins();
	QPoint bottomRight = QPoint(w->width(), w->height());
	bottomRight -= QPoint(marg.right() - 3, marg.bottom() - 3);

	QPoint topLeft = QPoint(marg.left() - 3, marg.top() - 3);

	if (!QRect(topLeft, bottomRight).contains(me->pos())) {
		return true;
	}

	return false;
}
bool BuildExpContainer::ProcessSelection(QMouseEvent *me) {
	auto w = this;

	if (IsIgnoreArea(me)) {
		return false;
	}

	emit _bw->ElementSelected(w);
	return true;
}
bool BuildExpContainer::ProcessDrag(QMouseEvent *me) {
	auto w = this;

	if (IsIgnoreArea(me)) {
		return false;
	}

	if (pressButton == Qt::LeftButton) {
		auto margins = w->contentsMargins();
		auto rect = w->rect();

		QPoint startPoint;
		startPoint.setX(margins.left() - 1);
		startPoint.setY(margins.top() - 1);
		QPoint endPoint;
		endPoint.setX(rect.width() - margins.right() + 1);
		endPoint.setY(rect.height() - margins.bottom());

		QUuid id = _bw->GetId(w);

		auto mime = new QMimeData;
		mime->setData(CONTAINER_MIME_TYPE, id.toByteArray());

		auto pixmap = w->grab(QRect(startPoint, endPoint));
		pixmap = pixmap.scaledToHeight((endPoint.y() - startPoint.y()) / 2, Qt::SmoothTransformation);

		QDrag *drag = new QDrag(this);
		drag->setMimeData(mime);
		drag->setPixmap(pixmap);
		drag->setHotSpot((pressPoint - QPoint(margins.left(), margins.top())) / 2);

		Qt::DropAction dropAction = drag->exec(Qt::MoveAction, Qt::MoveAction);
		return true;
	}
	
	return false;
}

BuilderWidget::BuilderWidget(QWidget *parent) :
	QFrame(parent)
{
	background.currentArea = 0;
	ui.selectOverlay = 0;
	selectedBc = 0;

	connect(this, &BuilderWidget::EnqueueUpdateBackgroundMap,
		this, &BuilderWidget::UpdateBackgroundMap, Qt::QueuedConnection);
	connect(this, &BuilderWidget::ElementSelected,
		this, &BuilderWidget::HandleSelection, Qt::QueuedConnection);

	OBJ_NAME(this, "build-exp-holder");
	this->setAcceptDrops(true);

	ui.elementsLay = 0;

	InitContainer();
	InitWidgets();
	UpdateLines();
	PlaceWidgets();
	emit EnqueueUpdateBackgroundMap();
}
const BuilderContainer& BuilderWidget::GetContainer() {
	return container;
}
void BuilderWidget::InitContainer() {
	container.type = BuilderContainer::SET;
	container.repeats = 1;
	container.w = 0;
	container.elem.ptr = 0;
}
void BuilderWidget::PlaceWidgets() {
	if (!ui.elementsLay) {
		ui.elementsLay = NO_MARGIN(new QGridLayout(this));
		ui.elementsLay->setSpacing(BUILDER_ELEMENT_SPACING);
	}
	while (ui.elementsLay->count()) {
		auto item = ui.elementsLay->itemAt(0);
		ui.elementsLay->removeItem(item);
		
		delete item;
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
					it->w = CreateBuildExpElementWidget(*it, it->id);
					it->w->hide();
				}
				break;

			case BuilderContainer::SET:
				for (auto cIt = it->elements.begin(); cIt != it->elements.end(); ++cIt) {
					if (!cIt->w) {
						cIt->w = CreateBuildExpElementWidget(*cIt, cIt->id);
					}
				}
				if (!it->w) {
					it->w = CreateBuildExpContainerWidget(*it, it->id);
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
QWidget* BuilderWidget::CreateBuildExpElementWidget(const BuilderContainer &bc, QUuid id) {
	if (bc.type != BuilderContainer::ELEMENT) {
		LOG() << "It is not an ELEMENT!";
		return 0;
	}

	auto *w = OBJ_NAME(new QFrame(this), "node-builder-element");
	w->installEventFilter(new ElementEventFilter(w, this));

	auto lay = NO_SPACING(NO_MARGIN(new QVBoxLayout(w)));

	QSpinBox *mult;
	QLabel *label;

	lay->addWidget(label = OBJ_NAME(new QLabel, "node-builder-label"));
	lay->addWidget(mult = OBJ_NAME(new QSpinBox(), "node-builder-multiplier-single"));

	label->setPixmap(bc.elem.ptr->GetImage());

	mult->setMinimum(1);
	mult->setMaximum(99999);
	mult->setValue(bc.repeats);

	auto disconnector = new Disconnector(mult);
	
	*disconnector <<
	connect(mult, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int val) {
		SetRepeats(id, val);
	});

	return w;
}
QWidget* BuilderWidget::CreateBuildExpContainerWidget(const BuilderContainer &bc, QUuid id) {
	if (bc.type != BuilderContainer::SET) {
		LOG() << "It is not a SET!";
		return 0;
	}

	auto cont = new BuildExpContainer(this, bc, id);

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
	if (e->mimeData()->hasFormat(ELEMENT_MIME_TYPE) || 
		e->mimeData()->hasFormat(CONTAINER_MIME_TYPE)) {
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
		if (!(e->mimeData()->hasFormat(ELEMENT_MIME_TYPE) ||
			e->mimeData()->hasFormat(CONTAINER_MIME_TYPE)) ) {
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
	if (e->mimeData()->hasFormat(CONTAINER_MIME_TYPE)) {
		if (!background.currentArea) {
			return;
		}

		auto area = background.currentArea;
		auto byteArray = e->mimeData()->data(CONTAINER_MIME_TYPE);
		auto id = QUuid(byteArray);
		BuilderContainer toMove;
		BuilderContainer *origin = 0;

		for (auto it = container.elements.begin(); it != container.elements.end(); ++it) {
			if (it->id == id) {
				toMove = *it;
				origin = &(*it);
				break;
			}

			bool found = false;
			for (auto cIt = it->elements.begin(); cIt != it->elements.end(); ++cIt) {
				if (cIt->id == id) {
					toMove = *cIt;
					origin = &(*cIt);
					found = true;
					break;
				}
			}
			if (found) {
				break;
			}
		}
		if (!origin) {
			return;
		}

		if (area->list == &origin->elements) {
			background.currentArea = 0;
			this->update();
			return;
		}

		toMove.id = QUuid::createUuid();
		toMove.w = 0;
		for (auto it = toMove.elements.begin(); it != toMove.elements.end(); ++it) {
			it->id = QUuid::createUuid();
			it->w = 0;
		}

		if (area->list) {

			if ( (area->list == &container.elements) || (toMove.type == BuilderContainer::ELEMENT) ) {
				area->list->insert(area->before, toMove);
			}
			else {
				for (auto it = toMove.elements.rbegin(); it != toMove.elements.rend(); ++it) {
					area->list->insert(area->before, *it);
				}
			}
		}
		else {
			BuilderContainer oldBc = *area->before;

			BuilderContainer newCont;
			newCont.type = BuilderContainer::SET;
			newCont.repeats = 1;
			newCont.id = QUuid::createUuid();
			newCont.w = 0;
			newCont.elem.ptr = 0;


			if (toMove.type == BuilderContainer::ELEMENT) {
				if (area->dropAction == BackgroundDescriptor::AFTER) {
					newCont.elements << oldBc;
				}

				newCont.elements << toMove;

				if (area->dropAction == BackgroundDescriptor::BEFORE) {
					newCont.elements << oldBc;
				}
			}
			else {
				if (area->dropAction == BackgroundDescriptor::AFTER) {
					newCont.elements << oldBc;
				}

				for (auto it = toMove.elements.begin(); it != toMove.elements.end(); ++it) {
					newCont.elements << *it;
				}

				if (area->dropAction == BackgroundDescriptor::BEFORE) {
					newCont.elements << oldBc;
				}
			}

			*area->before = newCont;
		}

		InitWidgets();
		DeleteContainer(origin);
	}

	if (e->mimeData()->hasFormat(ELEMENT_MIME_TYPE)) {
		if (!background.currentArea) {
			return;
		}

		auto area = background.currentArea;
		auto byteArray = e->mimeData()->data(ELEMENT_MIME_TYPE);
		auto emd = (ElementMimeData*)byteArray.data();

		if (area->list) {
			BuilderContainer bc;
			bc.type = BuilderContainer::ELEMENT;
			bc.repeats = 1;
			bc.id = QUuid::createUuid();
			bc.w = 0;
			bc.elem.ptr = emd->elem;
			bc.elem.name = emd->elem->GetFullName();
			bc.elem.ptr->CreateUserInput(bc.elem.input)->deleteLater();

			area->list->insert(area->before, bc);
		}
		else {
			BuilderContainer bc;
			bc.type = BuilderContainer::ELEMENT;
			bc.repeats = 1;
			bc.id = QUuid::createUuid();
			bc.w = 0;
			bc.elem.ptr = emd->elem;
			bc.elem.name = emd->elem->GetFullName();
			bc.elem.ptr->CreateUserInput(bc.elem.input)->deleteLater();

			BuilderContainer oldBc = *area->before;

			BuilderContainer newCont;
			newCont.type = BuilderContainer::SET;
			newCont.repeats = 1;
			newCont.id = QUuid::createUuid();
			newCont.w = 0;
			newCont.elem.ptr = 0;
			
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
void BuilderWidget::mouseReleaseEvent(QMouseEvent *e) {
	HandleSelection(0);
}
QUuid BuilderWidget::GetId(QWidget *w) {
	if (!w) {
		return QUuid();
	}

	QUuid ret;
	for (auto it = container.elements.begin(); it != container.elements.end(); ++it) {
		if (it->w == w) {
			ret = it->id;
			break;
		}

		if (it->type == BuilderContainer::SET) {
			bool found = false;

			for (auto cIt = it->elements.begin(); cIt != it->elements.end(); ++cIt) {
				if (cIt->w == w) {
					ret = cIt->id;
					found = true;
					break;
				}
			}

			if (found) {
				break;
			}
		}
	}
	return ret;
}
void BuilderWidget::HandleSelection(QWidget *w) {
	if (ui.selectOverlay) {
		ui.selectOverlay->deleteLater();
		ui.selectOverlay = 0;

		selectedBc = 0;
	}

	if (!w) {
		emit BuilderContainerSelected(selectedBc);
		return;
	}

	auto &bc(selectedBc);

	for (auto it = container.elements.begin(); it != container.elements.end(); ++it) {
		if (it->w == w) {
			bc = &(*it);
			break;
		}

		if (it->type == BuilderContainer::SET) {
			for (auto cIt = it->elements.begin(); cIt != it->elements.end(); ++cIt) {
				if (cIt->w == w) {
					bc = &(*cIt);
					break;
				}
			}
			if (bc) {
				break;
			}
		}
	}

	if (!bc) {
		emit BuilderContainerSelected(selectedBc);
		return;
	}

	int overlayPadding = 0;
	if (bc->type == BuilderContainer::SET) {
		overlayPadding = 1;
	}

	auto marg = w->contentsMargins();
	QPoint bottomRight = QPoint(w->width(), w->height());
	bottomRight -= QPoint(marg.right() + overlayPadding, marg.bottom());

	QPoint topLeft = QPoint(marg.left(), marg.top());
	
	ui.selectOverlay = OBJ_NAME(new QFrame(w), "select-element-overlay");
	ui.selectOverlay->setGeometry(QRect(topLeft, bottomRight));

	auto lay = NO_SPACING(NO_MARGIN(new QVBoxLayout(ui.selectOverlay)));
	lay->addWidget(OBJ_NAME(new QLabel(), "select-element-overlay-icon"));
	lay->addWidget(OBJ_NAME(new QLabel(bc->elem.name), "select-element-overlay-name"));

	ui.selectOverlay->show();
	ui.selectOverlay->raise();
	
	emit BuilderContainerSelected(selectedBc);
}
void BuilderWidget::RemoveSelection() {
	HandleSelection(0);
}
void BuilderWidget::DuplicateSelected() {
	if (!selectedBc) {
		return;
	}

	auto sourceBcPtr = selectedBc;
	auto copyBc = *selectedBc;

	copyBc.id = QUuid::createUuid();
	copyBc.w = 0;
	for(auto it = copyBc.elements.begin(); it != copyBc.elements.end(); ++it) {
		it->id = QUuid::createUuid();
		it->w = 0;
	}

	for (auto it = container.elements.begin(); it != container.elements.end(); ++it) {
		if (it->w == selectedBc->w) {
			container.elements.insert(++it, copyBc); --it;
			break;
		}

		bool found = false;
		for (auto cIt = it->elements.begin(); cIt != it->elements.end(); ++cIt) {
			if (cIt->w == selectedBc->w) {
				it->elements.insert(++cIt, copyBc); --cIt;
				found = true;
				break;
			}
		}
		if (found) {
			break;
		}
	}

	InitWidgets();
	UpdateLines();
	PlaceWidgets();
	emit EnqueueUpdateBackgroundMap();
}
void BuilderWidget::DeleteSelected() {
	DeleteContainer(selectedBc);
}
void BuilderWidget::DeleteContainer(BuilderContainer *bcPtr) {
	if (!bcPtr) {
		return;
	}

	auto toDeleteBc = bcPtr;

	HandleSelection(0);

	for (int i = 0; i < container.elements.count(); ++i) {
		auto &currentBc(container.elements[i]);

		if (&currentBc == toDeleteBc) {
			auto contW = qobject_cast<BuildExpContainer*>(currentBc.w);
			if (contW) {
				disconnect(contW, &BuildExpContainer::UpdateBackgroundMap,
					this, &BuilderWidget::UpdateBackgroundMap);
				disconnect(this, &BuilderWidget::RequestPlaceWidgets,
					contW, &BuildExpContainer::PlaceWidgets);
			}
			currentBc.w->deleteLater();
			container.elements.removeAt(i);

			break;
		}

		if (BuilderContainer::SET != currentBc.type) {
			continue;
		}

		bool found = false;
		
		for (int j = 0; j < currentBc.elements.count(); ++j) {
			auto &currentSubBc(currentBc.elements[j]);
			
			if (&currentSubBc == toDeleteBc) {
				auto contW = qobject_cast<BuildExpContainer*>(currentSubBc.w);
				if (contW) {
					disconnect(contW, &BuildExpContainer::UpdateBackgroundMap,
						this, &BuilderWidget::UpdateBackgroundMap);
					disconnect(this, &BuilderWidget::RequestPlaceWidgets,
						contW, &BuildExpContainer::PlaceWidgets);
				}
				currentSubBc.w->deleteLater();
				currentBc.elements.removeAt(j);

				if (currentBc.elements.count() == 1) {
					auto it = currentBc.elements.begin();

					auto backup = *it;
					backup.w->setParent(currentBc.w->parentWidget());

					currentBc.w->deleteLater();
					currentBc = backup;
				}

				found = true;
				break;
			}
		}

		if (found) {
			break;
		}
	}

	UpdateLines();
	PlaceWidgets();
	emit EnqueueUpdateBackgroundMap();
}
void BuilderWidget::SetupNewContainer(const BuilderContainer &bc) {
	while (container.elements.size()) {
		DeleteContainer(&(*container.elements.begin()));
	}

	container = bc;

	InitWidgets();
	UpdateLines();
	PlaceWidgets();
	emit EnqueueUpdateBackgroundMap();
}
void BuilderWidget::SetTotalRepeats(int val) {
	container.repeats = val;
}
void BuilderWidget::SetRepeats(QUuid id, int val) {
	for (auto it = container.elements.begin(); it != container.elements.end(); ++it) {
		if (id == it->id) {
			it->repeats = val;
			break;
		}

		if (BuilderContainer::SET != it->type) {
			continue;
		}

		bool found = false;

		for (auto cIt = it->elements.begin(); cIt != it->elements.end(); ++cIt) {
			if (id == cIt->id) {
				cIt->repeats = val;
				found = true;
				break;
			}
		}

		if (found) {
			break;
		}
	}
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