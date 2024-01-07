#ifndef DYNODEVICE_H
#define DYNODEVICE_H

#include <QObject>
#include <QIODevice>
#include <QSerialPort>
#include <QTimer>
#include <QFile>
#include <QTextStream>

//#define DEMOMODE_ENABLED

class DynoDevice : public QObject
{
		Q_OBJECT
	private:
		static DynoDevice * _instance;
		QSerialPort * _serialPort;
		QTimer * _serialPortWathdog;
		QSerialPort::SerialPortError _oldSerialError;

#ifdef DEMOMODE_ENABLED
		QTimer * _demoTimer;
		QFile * _demoFile;
		QTextStream * _demoTextStream;
#endif /* DEMOMODE_ENABLED */

		bool _ready;
		double _speed;
		int _gpsFix;
		int _gpsSIV;

		QString _dataLine;

		explicit DynoDevice(QObject *parent = nullptr);

	private slots:
		void _serialportWathdogTick(void);
		void _serialportDataReady(void);
#ifdef DEMOMODE_ENABLED
		void _demoTimerTick(void);
#endif /* DEMOMODE_ENABLED */
	public:
		static const int NoFix = 1;
		static const int Fix2D = 2;
		static const int Fix3D = 3;

		static DynoDevice * getInstance(void);

		void setPort(QString port);

	signals:
		void serialErrorChanged(QSerialPort::SerialPortError error);
		void serialPortChanged(QString name);
		void gpsStatusChanged(int fix, int siv);
		void speedChanged(double speed);
		void newData(double time, double speed);
};

#endif // DYNODEVICE_H
