#pragma once

#include <QWidget>
#include <QFrame>

#include <QGridLayout>

#include <QFlags>

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

protected:
	void paintEvent(QPaintEvent *e);

private:
	const BuilderContainer &bc;
};

class BuilderWidget : public QFrame {
	Q_OBJECT
public:
	BuilderWidget(QWidget *parent);

protected:
	void paintEvent(QPaintEvent *e);

private:
	void InitContainer();
	void PlaceWidgets();
	void UpdateLines();

	QWidget* CreateBuildExpElementWidget(const BuilderContainer&);
	QWidget* CreateBuildExpContainerWidget(const BuilderContainer&);
	
	BuilderContainer container;

	struct {
		QGridLayout *elementsLay;
	} ui;
};