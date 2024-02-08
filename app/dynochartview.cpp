#include "dynochartview.h"
#include <QValueAxis>
#include <QAbstractAxis>
#include <QToolTip>
#include "dynosettings.h"

DynoChartView::DynoChartView(QChart * chart, QWidget * parent) : QChartView(chart, parent) {
	_chart = chart;
	setToolTipDuration(99999);
	setCursor(QCursor(Qt::CrossCursor));
	setRenderHint(QPainter::Antialiasing);

	_infoBox = new DynoChartViewInfo(chart);
	_infoBox->setZValue(11);
	_infoBox->hide();

	_userInfoLabel = new QGraphicsTextItem();
	_userInfoLabel->setHtml("");

	_chart->scene()->addItem(_userInfoLabel);

	_chart->legend()->setVisible(false);
	//_chart->legend()->setAlignment(Qt::AlignBottom);
	//_chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);
	//_chart->setTitle("Dyno result");
	_chart->setMargins(QMargins(0, _userInfoLabel->boundingRect().height() + 5, 0, 0));
}

DynoChartView::~DynoChartView() {
	//delete _infoBox;
	delete _userInfoLabel;
}

void DynoChartView::updateUserInfo(QString text) {
	_userInfoLabel->setHtml(text);
	_userInfoLabel->setPos(width() - _userInfoLabel->boundingRect().width() - 10, 10);
	_chart->setMargins(QMargins(0, _userInfoLabel->boundingRect().height() + 5, 0, 0));
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

	_userInfoLabel->setPos(width() - _userInfoLabel->boundingRect().width() - 10, 10);

	//_legend->updateGeometry();
}
