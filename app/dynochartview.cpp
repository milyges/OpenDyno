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
	_chart->setMargins(QMargins(0, _userInfoLabel->boundingRect().height() + 1, 0, 0));
}

DynoChartView::~DynoChartView() {
	//delete _infoBox;
	removeRunInfo();
	delete _userInfoLabel;
}

void DynoChartView::updateUserInfo(QString text) {
	_userInfoLabel->setHtml(text);
	_userInfoLabel->setPos(width() - _userInfoLabel->boundingRect().width() - 10, 10);
	_chart->setMargins(QMargins(0, _userInfoLabel->boundingRect().height() + 5, 0, 0));
}

void DynoChartView::addRunInfo(Qt::PenStyle lineStyle, DynoRun * run) {
	DynoChartViewRunInfo * info = new DynoChartViewRunInfo(_chart, run, lineStyle);
	info->setPos(10, 10 + _runInfo.size() * (info->height()));
	_runInfo.append(info);
}

void DynoChartView::removeRunInfo() {
	while(!_runInfo.isEmpty()) {
		DynoChartViewRunInfo * item = _runInfo.last();
		_runInfo.removeLast();
		delete item;
	}
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

DynoChartViewRunInfo::DynoChartViewRunInfo(QChart * chart, DynoRun * run, Qt::PenStyle lineStyle) {
	_chart = chart;

	_text = new QGraphicsTextItem();
	_line = new QGraphicsLineItem();

	_text->setHtml(QString("<span style=\"color: red;\">%1KM@%2RPM</span> <span style=\"color: blue;\">%3Nm@%4RPM</span>").arg(run->powerMax(), 0, 'f', 1).arg(run->powerMaxRPM()).arg(run->torqueMax(), 0, 'f', 1).arg(run->torqueMaxRPM()));

	QPen p(lineStyle);
	p.setColor(Qt::red);
	_line->setPen(p);
	_line->setLine(5, 0, 25, 0);

	_chart->scene()->addItem(_text);
	_chart->scene()->addItem(_line);
}

DynoChartViewRunInfo::~DynoChartViewRunInfo() {
	_chart->scene()->removeItem(_text);
	_chart->scene()->removeItem(_line);
	delete _text;
	delete _line;
}

void DynoChartViewRunInfo::setPos(qreal x, qreal y) {
	_text->setPos(x + 30, y);
	_line->setPos(x, y + _text->boundingRect().height() / 2 - 1);
}

qreal DynoChartViewRunInfo::height() {
	return _text->boundingRect().height();
}
