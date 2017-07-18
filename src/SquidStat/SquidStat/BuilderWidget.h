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
#include <QUuid>

#include "AbstractBuilderElement.h"

#define ELEMENT_MIME_TYPE	"image/element"

struct ElementMimeData {
	AbstractBuilderElement *elem;
};

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

	qint32 repeats;
	Type type;
	QUuid id;

	QList<BuilderContainer> elements;
	LineDirection ld;
	QWidget *w;
	struct {
		AbstractBuilderElement *ptr;
		QString name;
		UserInput input;
	} elem;
};

class BuilderWidget;

class BuildExpContainer : public QFrame {
	Q_OBJECT
public:
	BuildExpContainer(BuilderWidget *parent, const BuilderContainer &cont, QUuid id);

signals:
	void UpdateBackgroundMap();

public slots:
	void PlaceWidgets();

protected:
	void paintEvent(QPaintEvent *e);
	void resizeEvent(QResizeEvent *e);
	void mousePressEvent(QMouseEvent *e);

private:
	const BuilderContainer &bc;

	struct {
		QVBoxLayout *elementsLay;
		QVBoxLayout *containerWdgLay;
	} ui;

	BuilderWidget *_bw;
};

class BuilderWidget : public QFrame {
	Q_OBJECT
public:
	BuilderWidget(QWidget *parent);
	
	const BuilderContainer& GetContainer();
	void SetupNewContainer(const BuilderContainer&);

	void RemoveSelection();

protected:
	void paintEvent(QPaintEvent *e);
	void dragEnterEvent(QDragEnterEvent *e);
	void dragLeaveEvent(QDragLeaveEvent *e);
	void dragMoveEvent(QDragMoveEvent *e);
	void dropEvent(QDropEvent *e);
	void resizeEvent(QResizeEvent *e);
	void mousePressEvent(QMouseEvent *e);

signals:
	void RequestPlaceWidgets();
	void EnqueueUpdateBackgroundMap();
	void ElementSelected(QWidget*);
	void BuilderContainerSelected(BuilderContainer*);

public slots:
	void DeleteContainer(BuilderContainer*);
	void DeleteSelected();
	void DuplicateSelected();
	void SetTotalRepeats(int);
	void SetRepeats(QUuid, int);

private slots:
	void UpdateBackgroundMap();
	void HandleSelection(QWidget*);

private:
	void InitContainer();
	void InitWidgets();
	void PlaceWidgets();
	void UpdateLines();
	QWidget* CreateBuildExpElementWidget(const BuilderContainer&, QUuid);
	QWidget* CreateBuildExpContainerWidget(const BuilderContainer&, QUuid);
	

	BuilderContainer container;
	BuilderContainer *selectedBc;

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
		QWidget *selectOverlay;
	} ui;
};