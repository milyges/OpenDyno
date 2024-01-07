#include "dynochartviewlegend.h"
#include <QChart>
#include <QPainter>

DynoChartViewLegend::DynoChartViewLegend(QChart * parent) : QGraphicsItem(parent) {
	_boundingRect = QRectF(0, 0, 1, 1);
	_chart = parent;
}

QRectF DynoChartViewLegend::boundingRect() const {
	return _boundingRect;
}

void DynoChartViewLegend::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
	Q_UNUSED(option);
	Q_UNUSED(widget);

	Q_UNUSED(painter);
	//painter->drawRect(_boundingRect);
}

void DynoChartViewLegend::updateGeometry() {
	prepareGeometryChange();
	_boundingRect = QRectF(10, 10, _chart->geometry().width() - 20, 40);
}
