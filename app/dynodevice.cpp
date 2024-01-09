#include "dynodevice.h"
#include <QDebug>

DynoDevice * DynoDevice::_instance = nullptr;

DynoDevice::DynoDevice(QObject *parent) : QObject{parent} {
	_serialPortWathdog = new QTimer(this);
	_serialPortWathdog->setInterval(1000);
	_serialPortWathdog->setSingleShot(false);
	connect(_serialPortWathdog, SIGNAL(timeout()), this, SLOT(_serialportWathdogTick()));

	_serialPort = new QSerialPort();
	_serialPort->setDataBits(QSerialPort::Data8);
	_serialPort->setFlowControl(QSerialPort::NoFlowControl);
	_serialPort->setParity(QSerialPort::NoParity);
	connect(_serialPort, SIGNAL(readyRead()), this, SLOT(_serialportDataReady()));

	_oldSerialError = QSerialPort::UnknownError;
	_ready = false;

	_dataLine = "";

#ifdef DEMOMODE_ENABLED
	_demoTimer = new QTimer();
	_demoTimer->setSingleShot(false);
	_demoTimer->setInterval(100);
	connect(_demoTimer, SIGNAL(timeout()), this, SLOT(_demoTimerTick()));

	_demoFile = nullptr;
	_demoTextStream = nullptr;
#endif
}

void DynoDevice::_serialportWathdogTick() {
	QSerialPort::SerialPortError err = _serialPort->error();

	if (err != _oldSerialError) {
		_oldSerialError = err;
		emit serialErrorChanged(err);
	}

	if (err != QSerialPort::NoError) {
		_ready = false;
		_gpsSIV = 0;
		_gpsFix = NoFix;
		_speed = 0;

		emit gpsStatusChanged(_gpsFix, _gpsSIV);
		emit speedChanged(_speed);

		//qDebug() << "Serial port Error" << err;

		_serialPort->clearError();

		if (_serialPort->isOpen()) {
			_serialPort->close();
		}
	}

	if (!_serialPort->isOpen()) {
		_serialPort->setBaudRate(QSerialPort::Baud115200);

		if (!_serialPort->open(QIODeviceBase::ReadWrite)) {
			//qDebug() << "Unable to open" << _serialPort->portName();
			return;
		}

		//qDebug() << "Port opened" << _serialPort->portName();
		_ready = true;
	}
}

void DynoDevice::_serialportDataReady() {
	if (!_ready) {
		_serialPort->readAll();
	}

	_dataLine += _serialPort->readLine();

	if (!_dataLine.endsWith("\r\n")) { /* To nie koniec lini */
		return;
	}
	if (_dataLine[0] != '+') { /* Nie linia z danymi, wypisujemy w debug i olewamy */
		qDebug() << _dataLine;
		_dataLine.clear();
		return;
	}

	QStringList data = _dataLine.split(";");
	double time, speed;
	int fix, siv;

	time = data[0].toULong() / 1000.0f;
	speed = (data[1].toULong() / 1000.0f) * 3.6f;
	siv = data[2].toULong();
	fix = data[3].toULong();

	_dataLine.clear();

	if ((_gpsSIV != siv) || (_gpsFix != fix)) {
		_gpsFix = fix;
		_gpsSIV = siv;
		emit gpsStatusChanged(_gpsFix, _gpsSIV);
	}

	if (_speed != speed) {
		_speed = speed;
		emit speedChanged(_speed);
	}

	emit newData(time, speed);
	//qDebug() << "time" << time << "speed" << speed << "fix" << fix << "siv" << siv;
}

#ifdef DEMOMODE_ENABLED
void DynoDevice::_demoTimerTick() {
	if (!_demoFile) {
		_demoFile = new QFile("/home/milyges/Dokumenty/opendyno/test3.dyno");
		_demoFile->open(QIODevice::ReadOnly | QIODevice::Text);
	}

	if (!_demoTextStream) {
		_demoTextStream = new QTextStream(_demoFile);
		qDebug() << _demoTextStream->readLine();
		_demoTimer->setInterval(100);
		_demoTimer->setSingleShot(false);
		_demoTimer->start();
	}

	QString line = _demoTextStream->readLine();
	if (line.isNull()) {
		_demoTimer->stop();
		_demoFile->close();

		delete _demoTextStream;
		delete _demoFile;

		_demoTextStream = nullptr;
		_demoFile = nullptr;
	}

	QStringList data = line.split(";");
	if (data.size() == 2) {
		emit newData(data[0].toDouble(), data[1].toDouble());
	}

}
#endif /* DEMOMODE_ENABLED */

DynoDevice * DynoDevice::getInstance() {
	if (!_instance) {
		_instance = new DynoDevice();
	}

	return _instance;
}

void DynoDevice::setPort(QString port) {
	if (_serialPort->isOpen()) {
		_serialPort->close();
	}

	_serialPort->setPortName(port);
	_serialPortWathdog->start();
	emit serialPortChanged(port);

#ifdef DEMOMODE_ENABLED
	_demoTimer->start(5000);
#endif /* DEMOMODE_ENABLED */
}

