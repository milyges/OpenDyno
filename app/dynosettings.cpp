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

QString DynoSettings::userInfo() {
	return _settings.value("user/info", "<b style=\"color: blue;\">OpenDyno Test</b><br><br><i>https://github.com/milyges/OpenDyno/</i>").toString();
}

QString DynoSettings::watermarkPath() {
	return _settings.value("user/watermark", "").toString();
}

double DynoSettings::correctionFactor() {
	return _settings.value("main/correctionFactor", 1.0f).toDouble();
}

double DynoSettings::filterPower()
{
	return _settings.value("main/filterPower", 4.0f).toDouble();
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

void DynoSettings::setUserInfo(QString text) {
	_settings.setValue("user/info", text);
}

void DynoSettings::setWatermark(QString path) {
	_settings.setValue("user/watermark", path);
}

void DynoSettings::setCorrectionFactor(double cf) {
	_settings.setValue("main/correctionFactor", cf);
}

void DynoSettings::setFilterPower(double fp) {
	_settings.setValue("main/filterPower", fp);
}

DynoSettings * DynoSettings::getInstance() {
	if (!_instance) {
		_instance = new DynoSettings();
	}

	return _instance;
}
