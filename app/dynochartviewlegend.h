#ifndef DYNOCHARTVIEWLEGEND_H
#define DYNOCHARTVIEWLEGEND_H

#include <QGraphicsItem>
#include <QObject>
#include <QChart>

class DynoChartViewLegend : public QGraphicsItem {
	private:
		QRectF _boundingRect;
		QChart * _chart;
	public:
		DynoChartViewLegend(QChart * parent);

		QRectF boundingRect() const;
		void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
		void updateGeometry();
};

#endif // DYNOCHARTVIEWLEGEND_H
