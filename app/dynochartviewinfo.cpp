#include "dynochartviewinfo.h"
#include <QPainter>
#include <QSplineSeries>
#include <QValueAxis>

qreal DynoChartViewInfo::_getValueAt(QSplineSeries * s, qreal x) {
	QList<QPointF> points = s->points();
	int i;
	for (i = 0; i < points.size(); i++) {
		if (points[i].x() == x) {
			return points[i].y();
		}
		else if ((i > 0) && (points[i - 1].x() < x) && (points[i].x() > x)) {
			return points[i - 1].y() + ((points[i].y() - points[i - 1].y()) / (points[i].x() - points[i - 1].x())) * (x - points[i - 1].x());
		}
	}
	return 0;
}

DynoChartViewInfo::DynoChartViewInfo(QChart * parent) : QGraphicsItem(parent) {
	_chart = parent;
	_boxRect = QRectF(5, 5, 210, 210);
	_textRect = QRectF(10, 10, 200, 200);
	_boundRect = _boxRect;
}

QRectF DynoChartViewInfo::boundingRect() const {
	return _boundRect;
}


void DynoChartViewInfo::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
	Q_UNUSED(option)
	Q_UNUSED(widget)
	int i;
	qreal yoffset;
	QFontMetrics fm(_font);
	QPen p;

	/* To jest do poprawy, napisane tylko tak żeby działało */
	//painter->drawLine(0, -1000, 0, 1000);
	painter->setBrush(QColor(252, 243, 207));
	painter->drawRect(_boxRect);

	for(i = 0; i < _texts.size(); i++) {
		yoffset = i * (fm.height() + 1);
		p.setColor(_texts[i].color);
		p.setStyle(_texts[i].style);
		painter->setPen(p);
		painter->drawLine(_textRect.x() + 1, _textRect.y() + yoffset + 1 + fm.height() / 2, _textRect.x() + 1 + fm.height() - 2, _textRect.y() + yoffset + 1 + fm.height() / 2);
		painter->setPen(Qt::black);
		painter->drawText(_textRect + QMarginsF(-(fm.height() + 3), -yoffset, 0, 0), _texts[i].text);
		painter->drawText(_textRect + QMarginsF(-(fm.height() + 3 + 85), -yoffset, 0, 0), QString("%1 %2").arg(_texts[i].value, 0, 'f', 1).arg(_texts[i].unit));
	}
}

void DynoChartViewInfo::setPoint(QPointF & p) {
	_texts.clear();
	DynoChartViewInfoText t;
	int lines = 1;
	QList<QAbstractSeries *> series = _chart->series();
	int i;

	t.text = "RPM";
	t.value = p.x();
	t.color = QColor(252, 243, 207);
	t.unit = "";
	_texts.append(t);


	for (i = 0; i < series.size(); i++) {
		QSplineSeries * s = (QSplineSeries *)series[i];
		if (!s->isVisible()) {
			continue;
		}

		lines++;

		t.text = s->name();
		t.value = _getValueAt(s, p.x());
		t.color = s->color();
		t.style = s->pen().style();

		_texts.append(t);
	}

	QFontMetrics fm(_font);
	_textRect = QRectF(10, 10, 200, lines * (fm.height() + 1));
	_boxRect = _textRect + QMarginsF(5, 5, 5, 5);
	_boundRect = _boxRect;
}

