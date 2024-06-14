#ifndef WNDSPEEDACC_H
#define WNDSPEEDACC_H

#include <QDialog>
#include <QChart>
#include <QChartView>
#include <QValueAxis>
#include <QSplineSeries>


namespace Ui {
	class WndSpeedAcc;
}

class WndSpeedAcc : public QDialog
{
		Q_OBJECT

	public:
		explicit WndSpeedAcc(QWidget * parent = nullptr);
		~WndSpeedAcc();

		QChart * getChart(void);
		QValueAxis * getAxisTime(void);
		QValueAxis * getAxisSpeed(void);
		QValueAxis * getAxisAcceleration(void);

	private:
		Ui::WndSpeedAcc * _ui;

		/* Wykres */
		QChart * _chart;
		QValueAxis * _axisTime;
		QValueAxis * _axisSpeed;
		QValueAxis * _axisAcceleration;
		QChartView * _chartView;
};

#endif // WNDSPEEDACC_H
