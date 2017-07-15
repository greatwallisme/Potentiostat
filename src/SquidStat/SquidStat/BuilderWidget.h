#pragma once

#include <QWidget>
#include <QFrame>

#include <QGridLayout>

#include <QFlags>

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QResizeEvent>

#include <QColor>

#include <QMap>

#define ELEMENT_MIME_TYPE	"image/element"

enum LineDirection_e : quint8 {
	LD_NONE = 0,
	LD_LEFT = 1,
	LD_RIGHT = 2,
	LD_TOP = 4,
	LD_BOTTOM = 8
};
typedef QFlags<LineDirection_e> LineDirection;

struct BuilderContainer {
	enum Type : qint32 {
		ELEMENT,
		SET
	};

	BuilderContainer(qint32 rep = 1, Type t = ELEMENT);

	qint32 repetition;
	Type type;

	QList<BuilderContainer> elements;
	LineDirection ld;
	QWidget *w;
};

class BuildExpContainer : public QFrame {
	Q_OBJECT
public:
	BuildExpContainer(QWidget *parent, const BuilderContainer &cont);

signals:
	void UpdateBackgroundMap();

public slots:
	void PlaceWidgets();

protected:
	void paintEvent(QPaintEvent *e);
	void resizeEvent(QResizeEvent *e);

private:
	const BuilderContainer &bc;

	struct {
		QVBoxLayout *elementsLay;
		QVBoxLayout *containerWdgLay;
	} ui;
};

class BuilderWidget : public QFrame {
	Q_OBJECT
public:
	BuilderWidget(QWidget *parent);

protected:
	void paintEvent(QPaintEvent *e);
	void dragEnterEvent(QDragEnterEvent *e);
	void dragLeaveEvent(QDragLeaveEvent *e);
	void dragMoveEvent(QDragMoveEvent *e);
	void dropEvent(QDropEvent *e);
	void resizeEvent(QResizeEvent *e);

signals:
	void RequestPlaceWidgets();
	void EnqueueUpdateBackgroundMap();

private slots:
	void UpdateBackgroundMap();

private:
	void InitContainer();
	void InitWidgets();
	void PlaceWidgets();
	void UpdateLines();
	QWidget* CreateBuildExpElementWidget(const BuilderContainer&);
	QWidget* CreateBuildExpContainerWidget(const BuilderContainer&);
	
	BuilderContainer container;
	struct BackgroundDescriptor {
		enum DragMoveAction : quint8 {
			ACCEPT,
			IGNORE
		};
		enum DropAction : quint8 {
			BEFORE,
			AFTER
		};
		struct DropArea {
			DragMoveAction dragMoveAction;
			QLine dropLine;
			QList<BuilderContainer>::iterator before;
			QList<BuilderContainer> *list;
			DropAction dropAction;
		};
		QMap<QColor, DropArea> areas;
		QImage map;
		
		DropArea *currentArea;
	} background;
	

	struct {
		QGridLayout *elementsLay;
	} ui;
};