#pragma once

#include <qwt_series_data.h>
#include "AbstractExperiment.h"

class ListSeriesData : public QwtSeriesData<QPointF> {
public:
	ListSeriesData(const DataStore &x, const DataStore &y);

	QPointF sample(size_t) const;
	size_t size() const;
	QRectF boundingRect() const;

private:
	const DataList &_x;
	const DataList &_y;
	QRectF _rect;
};