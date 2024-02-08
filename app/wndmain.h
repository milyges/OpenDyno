#ifndef WNDMAIN_H
#define WNDMAIN_H

#include <QMainWindow>
#include <QChart>
#include <QChartView>
#include <QValueAxis>
#include <QSplineSeries>

#include "dynochartview.h"
#include "dynorun.h"
#include "vehicleprofile.h"
#include "dynodevice.h"
#include <QSerialPort>


QT_BEGIN_NAMESPACE
namespace Ui { class WndMain; }
QT_END_NAMESPACE

class WndMain : public QMainWindow {
		Q_OBJECT

	public:
		WndMain(QWidget * parent = nullptr);
		~WndMain();

	private:
		const Qt::PenStyle _lineStyles[2] = { Qt::SolidLine, Qt::DashLine };

		Ui::WndMain * _ui;

		/* Wykres */
		QChart * _chart;
		QValueAxis * _axisRPM;
		QValueAxis * _axisPower;
		QValueAxis * _axisTorque;
		DynoChartView * _chartView;

		/* Pomiary */
		DynoRun * _currentRun;
		QList<DynoRun *> _runs;
		bool _runInPogress;

		/* Profil samochodu */
		VehicleProfile _currentProfile;

		QString _prevSavePath;


		DynoRun * _newDynoRun(void);

		uchar _visibleDataSeries(void);
		void _updateRunsInfoBox(void);
		void _updateRunInfo(int row, DynoRun * run);
		void _updateGuiState(void);
		void _updateProfileLabels(void);
		void _updateAxesMax(void);

	private slots:
		void _loadCurrentRun(void);
		void _saveCurrentRun(void);
		void _exportImage(void);
		void _loadOtherRun(void);
		void _removeRun(void);
		void _startStopRun(void);
		void _setVechicleProfile(void);
		void _dynoSettings(void);
		void _about(void);

		void _visiblaDataToogled(void);

		void _runStateChanged(DynoRun::DynoRunState state);

		void _devicePortChanged(QString name);
		void _deviceGpsStatusChanged(int fix, int siv);
		void _devicePortStatusChanged(QSerialPort::SerialPortError error);
		void _speedChanged(double speed);



};
#endif // WNDMAIN_H
