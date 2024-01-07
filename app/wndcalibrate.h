#ifndef WNDCALIBRATE_H
#define WNDCALIBRATE_H

#include <QDialog>

namespace Ui {
	class WndCalibrate;
}

class WndCalibrate : public QDialog {
		Q_OBJECT

	public:
		explicit WndCalibrate(QWidget *parent = nullptr);
		~WndCalibrate();

		float getRatio(void);

	private:
		Ui::WndCalibrate * _ui;

	private slots:
		void _speedChanged(double speed);
};

#endif // WNDCALIBRATE_H
