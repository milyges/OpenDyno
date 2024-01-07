#include "dynochartview.h"
#include <QValueAxis>
#include <QAbstractAxis>
#include <QToolTip>

DynoChartView::DynoChartView(QChart * chart, QWidget * parent) : QChartView(chart, parent) {
	_chart = chart;
	setToolTipDuration(99999);
	setCursor(QCursor(Qt::CrossCursor));

	_infoBox = new DynoChartViewInfo(chart);
	_infoBox->setZValue(11);
	_infoBox->hide();

	_chart->legend()->setVisible(true);
	_chart->legend()->setAlignment(Qt::AlignBottom);
	//_chart->legend()->detachFromChart();
	_chart->setTitle("Dyno result"); /* Obejscie żeby zrobić miejsce... */

	//_legend = new DynoChartViewLegend(chart);
	//_legend->setZValue(10);
	//_legend->show();
	//_legend->setPos(0, 0);
}

DynoChartView::~DynoChartView() {
	//delete _infoBox;
}

void DynoChartView::mouseMoveEvent(QMouseEvent * event) {
	QChartView::mouseMoveEvent(event);

	QList<QAbstractAxis *> axesX = _chart->axes(Qt::Horizontal);
	QList<QAbstractAxis *> axesY = _chart->axes(Qt::Vertical);

	if ((axesX.size() == 1) && (axesY.size() == 2)) {
		QValueAxis * axisX = (QValueAxis *)axesX.at(0);
		QValueAxis * axisYL = (QValueAxis *)axesY.at(0);

		/* Pobieramy wartość obrotów pod myszka */
		QPointF p =_chart->mapToValue(event->pos());

		if ((p.x() >= axisX->min()) && (p.x() < axisX->max()) &&
			(p.y() >= axisYL->min() && (p.y() < axisYL->max()))) {
			_infoBox->setPos(event->position());
			_infoBox->setPoint(p);
			_infoBox->show();
		}
		else {
			_infoBox->hide();
		}
	}
}

void DynoChartView::resizeEvent(QResizeEvent * event) {
	QChartView::resizeEvent(event);

	//_legend->updateGeometry();
}
