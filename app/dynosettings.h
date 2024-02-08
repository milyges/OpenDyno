#ifndef DYNOSETTINGS_H
#define DYNOSETTINGS_H

#include <QSettings>
#include "vehicleprofile.h"

class DynoSettings {
	private:
		static DynoSettings * _instance;
		QSettings _settings;

		DynoSettings();

	public:
		QString dataDir(void);
		QString gpsPort(void);
		int lossTime(void);
		VehicleProfile lastVehicleProfile(void);
		QString userInfo(void);
		QString watermarkPath(void);

		void setDataDir(QString dataDir);
		void setGpsPort(QString port);
		void setLastVehicleProfile(VehicleProfile vp);
		void setLossTime(int t);
		void setUserInfo(QString text);
		void setWatermark(QString path);

		static DynoSettings * getInstance(void);
};

#endif // DYNOSETTINGS_H
