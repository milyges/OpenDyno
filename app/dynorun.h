#ifndef DYNORUN_H
#define DYNORUN_H

#include <QObject>
#include <QChart>
#include <QValueAxis>
#include <QSplineSeries>

#include "dynorunresult.h"

class DynoRun : public QObject
{
		Q_OBJECT
	public:
		enum DynoRunState {
			RunInitial = 0,
			RunWaitForSpeed,
			RunCountdown,
			RunAccelerating,
			RunLosses,
			RunFinished,
			RunCanceled
		};

		static const uchar PowerVisible = 1;
		static const uchar PowerWheelVisible = 2;
		static const uchar PowerRawVisible = 4;
		static const uchar LossesVisible = 8;
		static const uchar LossesRawVisible = 16;
		static const uchar TorqueVisible = 32;

		explicit DynoRun(QChart * chart, QValueAxis * axisRpm, QValueAxis * axisPwr, QValueAxis * axisTrq, QObject *parent = nullptr);
		~DynoRun();

		DynoRun::DynoRunState state(void);

		bool loadFromFile(QString path);
		bool saveToFile(QString path);

		void startRun(void);
		void cancelRun(void);

		void redraw(void);
		void update(void);

		void setVisibleData(uchar v);
		void hide(void);
		void show(void);

		void setLineStyle(Qt::PenStyle style);
		void setParameters(float rpmRatio, int weight);

		double powerMax(void);
		int powerMaxRPM(void);

		double torqueMax(void);
		int torqueMaxRPM(void);

		double speedMax(void);
		int rpmMax(void);


	private:		
		QChart * _chart;

		QValueAxis * _axisRpm;
		QValueAxis * _axisPwr;
		QValueAxis * _axisTrq;

		QSplineSeries * _pwrRaw;
		QSplineSeries * _pwrWheel;
		QSplineSeries * _pwrTotal;
		QSplineSeries * _lossesRaw;
		QSplineSeries * _losses;
		QSplineSeries * _torque;

		DynoRunResult _result;
		int _drawState;
		double _pwrMax;
		int _pwrMaxRpm;

		double _torqueMax;
		int _torqueMaxRpm;

		int _rpmMax;
		double _speedMax;

		uchar _visibleData;

		DynoRunState _state;
		double _startTime;

		double calculateTorque(int rpm, double power);
		void _updateVisibeData(uchar v);

		void _finishRun(DynoRunState state);

	private slots:
		void _dynoNewData(double gpsTime, double speed);

	signals:
		void runStateChanged(DynoRun::DynoRunState state);
};

#endif // DYNORUN_H
