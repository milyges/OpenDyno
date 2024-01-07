#ifndef WNDPROFILE_H
#define WNDPROFILE_H

#include <QDialog>
#include "vehicleprofile.h"

namespace Ui {
	class WndProfile;
}

class WndProfile : public QDialog {
		Q_OBJECT

	public:
		explicit WndProfile(VehicleProfile & profile, QWidget *parent = nullptr);
		~WndProfile();

		VehicleProfile getValue(void);

	private:
		Ui::WndProfile * _ui;

	private slots:
		void _calibrateRatio(void);
};

#endif // WNDPROFILE_H
