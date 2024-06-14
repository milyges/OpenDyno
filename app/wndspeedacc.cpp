#include "wndspeedacc.h"
#include "ui_wndspeedacc.h"

WndSpeedAcc::WndSpeedAcc(QWidget *parent) : QDialog(parent), _ui(new Ui::WndSpeedAcc) {
	_ui->setupUi(this);

	_chart = new QChart();

	_axisTime = new QValueAxis();
	_axisSpeed = new QValueAxis();
	_axisAcceleration = new QValueAxis();

	_axisTime->setTitleText(tr("Time [s]"));
	_axisSpeed->setTitleText(tr("Speed [km/h]"));
	_axisAcceleration->setTitleText(tr("Acceleration [m/s^2]"));

	_axisTime->setTickCount(11);
	_axisTime->setMinorTickCount(1);
	_axisTime->setMinorGridLineVisible(true);
	_axisTime->setLabelFormat("%d");
	_axisTime->setMin(0);
	_axisTime->setMax(50);

	_axisSpeed->setTickCount(11);
	_axisSpeed->setMinorGridLineVisible(true);
	_axisSpeed->setMinorTickCount(1);
	_axisSpeed->setLabelFormat("%d");
	_axisSpeed->setMax(150);
	_axisSpeed->setMin(0);

	_axisAcceleration->setTickCount(11);
	_axisAcceleration->setMinorGridLineVisible(true);
	_axisAcceleration->setMinorTickCount(1);
	_axisAcceleration->setLabelFormat("%d");
	_axisAcceleration->setMax(7);
	_axisAcceleration->setMin(-5);

	_chart->addAxis(_axisTime, Qt::AlignBottom);
	_chart->addAxis(_axisSpeed, Qt::AlignLeft);
	_chart->addAxis(_axisAcceleration, Qt::AlignRight);

	_chartView = new QChartView(_chart, this);
	_ui->vlChart->addWidget(_chartView);

	_chart->legend()->setVisible(false);
}

WndSpeedAcc::~WndSpeedAcc() {	
	delete _ui;
	//delete _chart;
	//delete _chartView;
	delete _axisAcceleration;
	delete _axisSpeed;
	delete _axisTime;
}

QChart * WndSpeedAcc::getChart() {
	return _chart;
}

QValueAxis * WndSpeedAcc::getAxisTime() {
	return _axisTime;
}

QValueAxis * WndSpeedAcc::getAxisSpeed() {
	return _axisSpeed;
}

QValueAxis * WndSpeedAcc::getAxisAcceleration(){
	return _axisAcceleration;
}
