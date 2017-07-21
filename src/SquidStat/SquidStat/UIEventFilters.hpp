#pragma once

#include <qwt_plot.h>

#include <QEvent>
#include <QMouseEvent>

#include <QObject>
#include <QWidget>
#include <QScrollArea>
#include <QScrollBar>

#include <QSortFilterProxyModel>
#include <QGraphicsDropShadowEffect> 

#include <QtMath>

#include "AbstractBuilderElement.h"

#include <functional>

#define EXPERIMENT_VIEW_ALL_CATEGORY	"View All"

class MyScrollArea : public QScrollArea {
public:
	MyScrollArea() : pressed(false), dragged(false) {}

protected:
	void mousePressEvent(QMouseEvent *me) {
		pressed = true;
		dragged = false;
		pressButton = me->button();
		pressPoint = me->pos();

		me->ignore();
	}
	void mouseMoveEvent(QMouseEvent *me) {
		if (pressed) {
			QPoint pos = me->pos();
			QLine moveVector(pressPoint, pos);

			auto length = qSqrt(moveVector.dx() * moveVector.dx() + moveVector.dy() * moveVector.dy());

			if (dragged || (!dragged && (length > 3))) {
				dragged = true;

				Dragging(moveVector.dx(), moveVector.dy());

				pressPoint = pos;
				me->accept();
			}
			else {
				me->ignore();
			}
		}
		else {
			me->ignore();
		}
	}
	void mouseReleaseEvent(QMouseEvent *e) {
		if (dragged) {
			e->accept();
		}
		else {
			e->ignore();
		}

		pressed = false;
		dragged = false;
	}

private:
	void Dragging(int dx, int dy) {
		auto bar = this->verticalScrollBar();

		if (bar) {
			bar->setValue(bar->value() - dy);
		}
		bar = this->horizontalScrollBar();

		if (bar) {
			bar->setValue(bar->value() - dx);
		}
	}

	QPoint pressPoint;
	Qt::MouseButton pressButton;
	bool pressed;
	bool dragged;
};


class OverlayEventFilter : public QObject {
public:
	OverlayEventFilter(QObject *parent, AbstractBuilderElement *elem) :
		QObject(parent), _elem(elem), dragInAction(false) {}

	bool eventFilter(QObject *obj, QEvent *e) {
		if (e->type() == QEvent::MouseButtonPress) {
			QWidget *w = qobject_cast<QWidget*>(obj);
			if (!w) {
				return false;
			}

			QMouseEvent *me = (QMouseEvent*)e;

			if (me->button() == Qt::LeftButton) {
				auto margins = w->contentsMargins();
				auto rect = w->rect();

				QPoint startPoint;
				startPoint.setX(margins.left() - 1);
				startPoint.setY(margins.top() - 1);
				QPoint endPoint;
				endPoint.setX(rect.width() - margins.right());
				endPoint.setY(rect.height() - margins.bottom());

				ElementMimeData emd;
				emd.elem = _elem;

				auto mime = new QMimeData;
				mime->setData(ELEMENT_MIME_TYPE, QByteArray((char*)&emd, sizeof(ElementMimeData)));

				auto pixmap = w->grab(QRect(startPoint, endPoint));
				pixmap = pixmap.scaledToHeight((endPoint.y() - startPoint.y()) / 2, Qt::SmoothTransformation);

				QDrag *drag = new QDrag(obj);
				drag->setMimeData(mime);
				drag->setPixmap(pixmap);
				drag->setHotSpot((me->pos() - QPoint(margins.left(), margins.top())) / 2);

				dragInAction = true;
				Qt::DropAction dropAction = drag->exec(Qt::CopyAction, Qt::CopyAction);
				obj->deleteLater();
				w->parentWidget()->update();
				return true;
			}

			return false;
		}
		if (e->type() == QEvent::Enter) {
			return true;
		}
		if (e->type() == QEvent::Leave) {
			if (dragInAction) {
				return false;
			}
			obj->deleteLater();
			QWidget *w = qobject_cast<QWidget*>(obj);
			if (w) {
				w->parentWidget()->update();
			}
			return true;
		}
		return false;
	}
private:
	AbstractBuilderElement *_elem;
	bool dragInAction;
};
class ElementListEventFiler : public QObject {
public:
	ElementListEventFiler(QObject *parent, AbstractBuilderElement *elem) :
		QObject(parent), _elem(elem)
	{}

	bool eventFilter(QObject *obj, QEvent *e) {
		if (e->type() == QEvent::Enter) {
			QWidget *w = qobject_cast<QWidget*>(obj);

			if (!w) {
				return false;
			}

			auto marg = w->contentsMargins();

			auto additionalHeight = (w->height() - marg.bottom() - marg.top()) * 0.25;

			QPoint topLeft = QPoint(marg.left() - 1, marg.top() - 1);

			QPoint bottomRight = QPoint(w->width(), w->height() + additionalHeight);
			bottomRight -= QPoint(marg.right(), marg.bottom());

			auto overlay = OBJ_NAME(new QFrame(w->parentWidget()), "hover-element-overlay");
			overlay->installEventFilter(new OverlayEventFilter(overlay, _elem));
			overlay->setGeometry(QRect(topLeft + w->pos(), bottomRight + w->pos()));

			auto effect = new QGraphicsDropShadowEffect(overlay);
			effect->setOffset(3, 3);
			effect->setColor(QColor("#7f7f7f7f"));
			effect->setBlurRadius(5);
			overlay->setGraphicsEffect(effect);

			QLabel *iconLbl;
			QLabel *textLbl;

			auto lay = NO_SPACING(NO_MARGIN(new QVBoxLayout(overlay)));
			lay->addWidget(iconLbl = OBJ_NAME(new QLabel(), "hover-element-overlay-icon"));
			lay->addWidget(textLbl = OBJ_NAME(new QLabel(_elem->GetFullName()), "hover-element-overlay-name"));

			textLbl->setWordWrap(true);
			iconLbl->setPixmap(_elem->GetImage());

			overlay->show();
			overlay->raise();

			return true;
		}
		if (e->type() == QEvent::Leave) {
			return false;
		}
		return false;
	}

private:
	AbstractBuilderElement *_elem;

	//QWidget *overlay;
};

class ExperimentFilterModel : public QSortFilterProxyModel {
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
		QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

		if (!index.isValid()) {
			return false;
		}

		auto exp = index.data(Qt::UserRole).value<const AbstractExperiment*>();

		if (!exp) {
			return false;
		}

		QString pattern = filterRegExp().pattern();

		QString descriptionPlain = "";
		QXmlStreamReader xml("<i>" + exp->GetDescription() + "</i>");
		while (!xml.atEnd()) {
			if (xml.readNext() == QXmlStreamReader::Characters) {
				descriptionPlain += xml.text();
			}
		}

		bool validCategory = false;
		if (_category == EXPERIMENT_VIEW_ALL_CATEGORY) {
			validCategory = true;
		}
		else {
			validCategory = exp->GetCategory().contains(_category);
		}

		return (exp->GetShortName().contains(pattern, filterCaseSensitivity()) ||
			exp->GetFullName().contains(pattern, filterCaseSensitivity()) ||
			descriptionPlain.contains(pattern, filterCaseSensitivity())) && validCategory;
	}

public:
	void SetCurrentCategory(const QString &category) {
		_category = category;
		invalidateFilter();
	}

private:
	QString _category;
};

class PlotEventFilter : public QObject {
public:
	PlotEventFilter(QObject *parent, std::function<void(void)> lambda) : QObject(parent), _lambda(lambda) {}

	bool eventFilter(QObject *obj, QEvent *e) {
		if ((e->type() == QEvent::MouseButtonDblClick) || (e->type() == QEvent::MouseButtonPress)) {
			_lambda();
			return true;
		}
		return false;
	}

private:
	std::function<void(void)> _lambda;
};
class LegendEventFilter : public QObject {
public:
	LegendEventFilter(QObject *parent, QwtPlot *plot) : QObject(parent), _plot(plot) {}

	bool eventFilter(QObject *obj, QEvent *e) {
		if (e->type() == QEvent::MouseButtonDblClick) {
			obj->deleteLater();
			_plot->insertLegend(0);
			_plot->replot();
			return true;
		}
		return false;
	}

private:
	QwtPlot *_plot;
};
class PopupDialogEventFilter : public QObject {
public:
	PopupDialogEventFilter(QObject *parent, std::function<void(QEvent*, bool&)> lambda) : QObject(parent), _lambda(lambda) {}

	bool eventFilter(QObject *obj, QEvent *e) {
		if (e->type() == QEvent::KeyPress) {
			bool ret;
			_lambda(e, ret);
			return ret;
		}
		return false;
	}
private:
	std::function<void(QEvent*, bool&)> _lambda;
};

class PlotOverlayEventFilter : public QObject {
public:
	PlotOverlayEventFilter(QWidget *parent, QWidget *overlay) :
		QObject(parent), _parent(parent), _overlay(overlay) {}

	bool eventFilter(QObject *obj, QEvent *e) {
		if (e->type() == QEvent::Resize) {
			_overlay->setGeometry(0, 0, _parent->width(), _parent->height());
		}
		return false;
	}

private:
	QWidget *_parent;
	QWidget *_overlay;
};

class PlotDragOverlayEventFilter : public QObject {
public:
	PlotDragOverlayEventFilter(QObject *parent, std::function<bool(QObject*, QEvent*)> lambda) :
		QObject(parent), _lambda(lambda) {}

	bool eventFilter(QObject *obj, QEvent *e) {
		return _lambda(obj, e);
	}
private:
	std::function<bool(QObject*, QEvent*)> _lambda;
};