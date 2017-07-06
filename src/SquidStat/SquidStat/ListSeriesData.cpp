#include "ListSeriesData.h"

ListSeriesData::ListSeriesData(const DataStore &x, const DataStore &y) :
	_x(x.data), _y(y.data), _rect(x.min, y.min, x.max - x.min, y.max - y.min)
{
}
QPointF ListSeriesData::sample(size_t i) const {
	return QPointF(_x.at(i), _y.at(i));
}
size_t ListSeriesData::size() const {
	return qMin(_x.size(), _y.size());
}
QRectF ListSeriesData::boundingRect() const {
	return _rect;
}