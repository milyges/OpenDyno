#include "wndcalibrate.h"
#include "ui_wndcalibrate.h"
#include "dynodevice.h"

WndCalibrate::WndCalibrate(QWidget *parent) : QDialog(parent), _ui(new Ui::WndCalibrate) {
	_ui->setupUi(this);
	connect(DynoDevice::getInstance(), SIGNAL(speedChanged(double)), this, SLOT(_speedChanged(double)));
}

WndCalibrate::~WndCalibrate() {
	delete _ui;
}

float WndCalibrate::getRatio() {
	return _ui->sbRPM->value() / _ui->sbSpeed->value();
}

void WndCalibrate::_speedChanged(double speed) {
	_ui->sbSpeed->setValue(speed);
}
