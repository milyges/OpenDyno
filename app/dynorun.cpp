#include "dynorun.h"
#include <QFile>
#include "dynosettings.h"
#include "dynodevice.h"

DynoRun::DynoRun(QChart * chart, QValueAxis * axisRpm, QValueAxis * axisPwr, QValueAxis * axisTrq, QObject *parent): QObject{parent} {
	_chart = chart;
	_axisRpm = axisRpm;
	_axisPwr = axisPwr;
	_axisTrq = axisTrq;

	_pwrRaw = new QSplineSeries();
	_pwrWheel = new QSplineSeries();
	_pwrTotal = new QSplineSeries();
	_losses = new QSplineSeries();
	_lossesRaw = new QSplineSeries();
	_torque = new QSplineSeries();

	_chart->addSeries(_pwrRaw);
	_pwrRaw->setName(tr("Power (raw)"));
	_pwrRaw->attachAxis(_axisRpm);
	_pwrRaw->attachAxis(_axisPwr);
	_pwrRaw->setColor(QColorConstants::DarkYellow);

	_chart->addSeries(_pwrWheel);
	_pwrWheel->setName(tr("Power (wheel)"));
	_pwrWheel->attachAxis(_axisRpm);
	_pwrWheel->attachAxis(_axisPwr);
	_pwrWheel->setColor(QColorConstants::Cyan);

	_chart->addSeries(_pwrTotal);
	_pwrTotal->setName(tr("Power"));
	_pwrTotal->attachAxis(_axisRpm);
	_pwrTotal->attachAxis(_axisPwr);
	_pwrTotal->setColor(QColorConstants::Red);

	_chart->addSeries(_losses);
	_losses->setName(tr("Losses"));
	_losses->attachAxis(_axisRpm);
	_losses->attachAxis(_axisPwr);
	_losses->setColor(QColorConstants::Green);

	_chart->addSeries(_lossesRaw);
	_lossesRaw->setName(tr("Losses (raw)"));
	_lossesRaw->attachAxis(_axisRpm);
	_lossesRaw->attachAxis(_axisPwr);
	_lossesRaw->setColor(QColorConstants::Magenta);

	_chart->addSeries(_torque);
	_torque->setName(tr("Torque"));
	_torque->attachAxis(_axisRpm);
	_torque->attachAxis(_axisTrq);
	_torque->setColor(QColorConstants::Blue);

	_result = DynoRunResult();

	_pwrMax = 0;
	_pwrMaxRpm = 0;
	_torqueMax = 0;
	_torqueMaxRpm = 0;
	_rpmMax = 0;
	_speedMax = 0;
}

DynoRun::~DynoRun() {
	disconnect();

	delete _pwrRaw;
	delete _pwrWheel;
	delete _pwrTotal;
	delete _losses;
	delete _lossesRaw;
	delete _torque;
}

DynoRun::DynoRunState DynoRun::state() {
	return _state;
}

bool DynoRun::loadFromFile(QString path) {
	QFile f(path);
	QStringList s;

	if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return false;
	}

	QTextStream stream(&f);

	/* Odczytujemy nagłówek */
	s = stream.readLine().split(";");
	if (s[0] != "opendynofile") {
		f.close();
		return false;
	}

	/* Parametry */
	_result.setParameters(s[1].toFloat(), s[2].toInt());

	/* Czas + prędkość */
	while (!stream.atEnd()) {
		double time, speed;
		s = stream.readLine().split(";");
		time = s[0].toDouble();
		speed = s[1].toDouble();

		_result.addData(time, speed);
	}

	f.close();

	_state = RunFinished;
	redraw();
	return true;
}

bool DynoRun::saveToFile(QString path) {
	QFile f(path);
	if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return false;
	}

	QTextStream stream(&f);

	stream << QString("opendynofile;%1;%2\n").arg(_result.rpmRatio(), 0, 'f', 6).arg(_result.weight());

	int i;
	for (i = 0; i < _result.itemsCount(); i++)	 {
		stream << QString("%1;%2\n").arg(_result.item(i)->time).arg(_result.item(i)->speed);
	}

	f.close();
	return true;
}

void DynoRun::startRun() {
	_state = RunWaitForSpeed;
	emit runStateChanged(_state);
	connect(DynoDevice::getInstance(), SIGNAL(newData(double,double)), this, SLOT(_dynoNewData(double,double)));
}

void DynoRun::cancelRun() {
	_finishRun(RunCanceled);
}

void DynoRun::redraw() {
	DynoRunResultItem * item;
	double power, loss, torque, powerRaw, powerTotal;

	/* Czyścimy serie danych */
	_pwrRaw->clear();
	_pwrWheel->clear();
	_pwrTotal->clear();
	_losses->clear();
	_lossesRaw->clear();
	_torque->clear();

	_pwrMax = 0;
	_pwrMaxRpm = 0;
	_torqueMax = 0;
	_torqueMaxRpm = 0;
	_rpmMax = 0;
	_speedMax = 0;

	for(int i = 20; i < _result.resultsCount() - 5; i++) {
		item = _result.item(i);

		power = item->powerKwFiltered * 1.36f;
		powerRaw = item->powerKw * 1.36f;
		loss = _result.lossAt(item->rpm) * 1.36f;
		powerTotal = loss + power;
		torque = calculateTorque(item->rpm, powerTotal / 1.36);

		if (item->rpm >= _rpmMax) {
			_rpmMax = item->rpm;
		}

		if (item->speed >= _speedMax) {
			_speedMax = item->speed;
		}

		if (power + loss >= _pwrMax) {
			_pwrMaxRpm = item->rpm;
			_pwrMax = power + loss;
			//_pwrTotal->setName(QString("Power [%1KM@%2]").arg(_pwrMax, 0, 'f', 1).arg(item->rpm()));
		}

		if (torque >= _torqueMax) {
			_torqueMaxRpm = item->rpm;
			_torqueMax = torque;
			//_torque->setName(QString("Torque [%1Nm@%2]").arg(torque, 0, 'f', 1).arg(item->rpm()));
		}

		/* Skalowanie osi */
		if (powerRaw + 40 >= _axisPwr->max()) {
			_axisPwr->setMax(powerRaw + 40);
		}

		if (powerTotal + 40 >= _axisPwr->max()) {
			_axisPwr->setMax(powerTotal + 40);
		}

		if (torque + 60 >= _axisTrq->max()) {
			_axisTrq->setMax(torque + 60);
		}

		*_pwrRaw << QPointF(item->rpm, powerRaw);
		*_pwrWheel << QPointF(item->rpm, power);
		*_pwrTotal << QPointF(item->rpm, powerTotal);
		*_losses << QPointF(item->rpm, loss);
		*_torque << QPointF(item->rpm, torque);
	}

	/* Surowe straty */
	for (int i = _result.resultsCount() + 10; i < _result.itemsCount(); i++) {
		item = _result.item(i);
		loss = -item->powerKwFiltered * 1.36f;
		*_lossesRaw << QPointF(item->rpm, loss);
	}
}

void DynoRun::update() {

}

void DynoRun::setVisibleData(uchar v) {
	_visibleData = v;
	_updateVisibeData(v);
}

void DynoRun::hide() {
	_updateVisibeData(_visibleData);
}

void DynoRun::show() {
	_updateVisibeData(_visibleData);
}

void DynoRun::setLineStyle(Qt::PenStyle style) {
	QPen pen = QPen(style);
	pen.setWidthF(1);

	_pwrRaw->setPen(pen);
	_pwrRaw->setColor(QColorConstants::DarkYellow);

	_pwrWheel->setPen(pen);
	_pwrWheel->setColor(QColorConstants::Cyan);

	_pwrTotal->setPen(pen);
	_pwrTotal->setColor(QColorConstants::Red);

	_losses->setPen(pen);
	_losses->setColor(QColorConstants::Green);

	_lossesRaw->setPen(pen);
	_lossesRaw->setColor(QColorConstants::Magenta);

	_torque->setPen(pen);
	_torque->setColor(QColorConstants::Blue);
}

void DynoRun::setParameters(float rpmRatio, int weight) {
	_result.setParameters(rpmRatio, weight);
	redraw();
}

double DynoRun::powerMax() {
	return _pwrMax;
}

int DynoRun::powerMaxRPM() {
	return _pwrMaxRpm;
}

double DynoRun::torqueMax() {
	return _torqueMax;
}

int DynoRun::torqueMaxRPM() {
	return _torqueMaxRpm;
}

double DynoRun::speedMax() {
	return _speedMax;
}

int DynoRun::rpmMax() {
	return _rpmMax;
}

double DynoRun::calculateTorque(int rpm, double power) {
	return power * 9549.3f / rpm;
}

void DynoRun::_updateVisibeData(uchar v) {
	if ((v & PowerVisible) == PowerVisible) {
		_pwrTotal->show();
	}
	else {
		_pwrTotal->hide();
	}

	if ((v & PowerRawVisible) == PowerRawVisible) {
		_pwrRaw->show();
	}
	else {
		_pwrRaw->hide();
	}

	if ((v & PowerWheelVisible) == PowerWheelVisible) {
		_pwrWheel->show();
	}
	else {
		_pwrWheel->hide();
	}

	if ((v & LossesVisible) == LossesVisible) {
		_losses->show();
	}
	else {
		_losses->hide();
	}

	if ((v & LossesRawVisible) == LossesRawVisible) {
		_lossesRaw->show();
	}
	else {
		_lossesRaw->hide();
	}

	if ((v & TorqueVisible) == TorqueVisible) {
		_torque->show();
	}
	else {
		_torque->hide();
	}
}

void DynoRun::_finishRun(DynoRunState state) {
	_state = state;
	disconnect(DynoDevice::getInstance(), SIGNAL(newData(double,double)), this, SLOT(_dynoNewData(double,double)));
	emit runStateChanged(_state);
	redraw();
}

void DynoRun::_dynoNewData(double gpsTime, double speed) {
	static int ticks = 0;

	if ((_state == RunWaitForSpeed) && (speed >= 0.0f)) {
		_state = RunCountdown;
		ticks = -30; /* Czekamy 3 sek do początku pomiaru */
		emit runStateChanged(_state);
	}

	if ((_state == RunCountdown) && (ticks >= 0)) { /* Odliczanie do początku pomiaru */
		_startTime = gpsTime;
		_state = RunAccelerating;
		ticks = 0;
		emit runStateChanged(_state);
	}

	/* Przyśpieszanie */
	if (_state == RunAccelerating) {
		if (_result.lossesCount() > 0) {
			_state = RunLosses;
			emit runStateChanged(_state);
		}

		_result.addData(gpsTime - _startTime, speed);
	}
	else if (_state == RunLosses) { /* Pomiar strat */
		/* Straty obliczamy podaną ilośc czasu [20sek, zmieniane w ustawieniach] */
		if (_result.lossesCount() > DynoSettings::getInstance()->lossTime() * 10) {
			_finishRun(RunFinished);
		}
		_result.addData(gpsTime - _startTime, speed);
	}

	ticks++;
	if ((ticks % 10) == 0) {
		qDebug() << "redraw; time" << gpsTime - _startTime << "speed" << speed;
		redraw();
	}
}
