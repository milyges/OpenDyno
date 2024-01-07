#ifndef DYNOCHARTVIEWINFO_H
#define DYNOCHARTVIEWINFO_H

#include <QGraphicsItem>
#include <QWidget>
#include <QChart>
#include <QFont>
#include <QSplineSeries>
#include <QColor>

class DynoChartViewInfoText {
	public:
		QString text;
		float value;
		QString unit;
		QColor color;
		Qt::PenStyle style;
};

class DynoChartViewInfo : public QGraphicsItem {
	private:
		QChart * _chart;
		QRectF _boxRect;
		QRectF _textRect;
		QRectF _boundRect;
		//QString _text;
		QList<DynoChartViewInfoText> _texts;
		QFont _font;

		qreal _getValueAt(QSplineSeries * s, qreal x);
	public:
		DynoChartViewInfo(QChart * parent);

		//void updateGeometry();
		QRectF boundingRect() const;
		void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
		void setPoint(QPointF & p);
};

#endif // DYNOCHARTVIEWINFO_H
