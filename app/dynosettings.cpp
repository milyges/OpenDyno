#include "dynosettings.h"
#include <QStandardPaths>
#include <QDir>

DynoSettings * DynoSettings::_instance = nullptr;

DynoSettings::DynoSettings() {

}

QString DynoSettings::dataDir() {
	return _settings.value("main/dataDir", QString(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + "opendyno")).toString();
}

QString DynoSettings::gpsPort() {
	return _settings.value("main/gpsPort", "").toString();
}

int DynoSettings::lossTime() {
	return _settings.value("main/lossTime", 20).toInt();
}

VehicleProfile DynoSettings::lastVehicleProfile() {
	VehicleProfile vp;
	vp.airPress = _settings.value("profile/airPress", QVariant(1013)).toInt();
	vp.airTemp = _settings.value("profile/airTemp", QVariant(20)).toFloat();
	vp.rpmRatio = _settings.value("profile/rpmRatio", QVariant(20.0f)).toFloat();
	vp.weight = _settings.value("profile/weight", QVariant(1000)).toInt();
	return vp;
}

void DynoSettings::setDataDir(QString dataDir) {
	_settings.setValue("main/dataDir", dataDir);
}

void DynoSettings::setGpsPort(QString port) {
	_settings.setValue("main/gpsPort", port);
}

void DynoSettings::setLastVehicleProfile(VehicleProfile vp) {
	_settings.setValue("profile/airPress", vp.airPress);
	_settings.setValue("profile/airTemp", vp.airTemp);
	_settings.setValue("profile/rpmRatio", vp.rpmRatio);
	_settings.setValue("profile/weight", vp.weight);
}

void DynoSettings::setLossTime(int t) {
	_settings.setValue("main/lossTime", t);
}

DynoSettings * DynoSettings::getInstance() {
	if (!_instance) {
		_instance = new DynoSettings();
	}

	return _instance;
}
