#include "wndprofile.h"
#include "wndcalibrate.h"
#include "ui_wndprofile.h"

WndProfile::WndProfile(VehicleProfile & profile, QWidget *parent) : QDialog(parent), _ui(new Ui::WndProfile) {
	_ui->setupUi(this);

	_ui->sbPressure->setValue(profile.airPress);
	_ui->sbRpmRatio->setValue(profile.rpmRatio);
	_ui->sbTemperature->setValue(profile.airTemp);
	_ui->sbWeight->setValue(profile.weight);

	connect(_ui->bntCal, SIGNAL(clicked(bool)), this, SLOT(_calibrateRatio()));
}

WndProfile::~WndProfile() {
	delete _ui;
}

VehicleProfile WndProfile::getValue() {
	VehicleProfile v;
	v.airPress = _ui->sbPressure->value();
	v.rpmRatio = _ui->sbRpmRatio->value();
	v.airTemp = _ui->sbTemperature->value();
	v.weight = _ui->sbWeight->value();
	return v;
}

void WndProfile::_calibrateRatio() {
	WndCalibrate w(this);

	if (w.exec() == QDialog::Accepted) {
		_ui->sbRpmRatio->setValue(w.getRatio());
	}
}
