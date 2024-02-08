#ifndef WNDSETTINGS_H
#define WNDSETTINGS_H

#include <QDialog>

namespace Ui {
	class WndSettings;
}

class WndSettings : public QDialog {
		Q_OBJECT

	public:
		explicit WndSettings(QWidget *parent = nullptr);
		~WndSettings();

		QString dataDir(void);
		QString gpsPort(void);
		int lossTime(void);
		QString userInfo(void);
		QString watermarkFile(void);

	private:
		Ui::WndSettings * _ui;

	private slots:
		void _browseDir(void);
		void _browseWatermark(void);
};

#endif // WNDSETTINGS_H
