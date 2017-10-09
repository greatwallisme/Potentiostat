#pragma once

#include <QFrame>
#include <QString>
#include <QPushButton>
#include <QModelIndex>

class HwListButton : public QFrame {
	Q_OBJECT
public:
	enum State : char {
		ACTIVE,
		STOPPED,
		MIDDLE
	};

	HwListButton(const QModelIndex&, const QString &);

	void SetColorState(State);

signals:
	void Clicked(const QModelIndex&);

private:
	QPushButton *pbt;
	QModelIndex index;
};