#include "wndmain.h"
#include "ui_wndmain.h"
#include <QFileDialog>
#include <QMessageBox>
#include "dynosettings.h"
#include "wndprofile.h"
#include "wndsettings.h"
#include "dynodevice.h"
#include "wndabout.h"
#include "dynochartseriespen.h"

WndMain::WndMain(QWidget * parent) : QMainWindow(parent), _ui(new Ui::WndMain) {
	_ui->setupUi(this);

	_chart = new QChart();

	_axisRPM = new QValueAxis();
	_axisPower = new QValueAxis();
	_axisTorque = new QValueAxis();

	_axisRPM->setTitleText(tr("RPM"));
	_axisPower->setTitleText(tr("Power [KM]"));
	_axisTorque->setTitleText(tr("Torque [Nm]"));

	_axisRPM->setTickCount(11);
	_axisRPM->setMinorTickCount(1);
	_axisRPM->setMinorGridLineVisible(true);
	_axisRPM->setLabelFormat("%d");
	_axisRPM->setMin(1500);
	_axisRPM->setMax(4000);

	_axisPower->setTickCount(11);
	_axisPower->setMinorGridLineVisible(true);
	_axisPower->setMinorTickCount(1);
	_axisPower->setLabelFormat("%d");
	_axisPower->setMax(100);

	_axisTorque->setTickCount(11);
	_axisTorque->setMinorGridLineVisible(true);
	_axisTorque->setMinorTickCount(1);
	_axisTorque->setLabelFormat("%d");
	_axisTorque->setMax(100);

	_chart->addAxis(_axisRPM, Qt::AlignBottom);
	_chart->addAxis(_axisPower, Qt::AlignLeft);
	_chart->addAxis(_axisTorque, Qt::AlignRight);

	_chartView = new DynoChartView(_chart, this);
	_chartView->updateUserInfo(DynoSettings::getInstance()->userInfo());
	_ui->vlChart->addWidget(_chartView);

	_currentRun = nullptr;
	_runs = QList<DynoRun *>();
	_runInPogress = false;

	_currentProfile = DynoSettings::getInstance()->lastVehicleProfile();

	_prevSavePath = DynoSettings::getInstance()->dataDir();

	connect(_ui->actionRunStart, SIGNAL(triggered(bool)), this, SLOT(_startStopRun()));
	connect(_ui->actionRunLoad, SIGNAL(triggered(bool)), this, SLOT(_loadCurrentRun()));
	connect(_ui->actionRunSave, SIGNAL(triggered(bool)), this, SLOT(_saveCurrentRun()));
	connect(_ui->actionRunExport, SIGNAL(triggered(bool)), this, SLOT(_exportImage()));

	connect(_ui->actionViewLosses, SIGNAL(toggled(bool)), this, SLOT(_visiblaDataToogled()));
	connect(_ui->actionViewLossesRaw, SIGNAL(toggled(bool)), this, SLOT(_visiblaDataToogled()));
	connect(_ui->actionViewPower, SIGNAL(toggled(bool)), this, SLOT(_visiblaDataToogled()));
	connect(_ui->actionViewPowerOnWheels, SIGNAL(toggled(bool)), this, SLOT(_visiblaDataToogled()));
	connect(_ui->actionViewPowerOnWheelsRaw, SIGNAL(toggled(bool)), this, SLOT(_visiblaDataToogled()));
	connect(_ui->actionViewTorque, SIGNAL(toggled(bool)), this, SLOT(_visiblaDataToogled()));

	connect(_ui->actionSettingsProfile, SIGNAL(triggered(bool)), this, SLOT(_setVechicleProfile()));
	connect(_ui->actionSettingsDyno, SIGNAL(triggered(bool)), this, SLOT(_dynoSettings()));

	connect(_ui->actionHelpAbout, SIGNAL(triggered(bool)), this, SLOT(_about()));

	connect(_ui->bntLoadRun, SIGNAL(pressed()), this, SLOT(_loadOtherRun()));
	connect(_ui->bntRemoveRun, SIGNAL(pressed()), this, SLOT(_removeRun()));

	connect(DynoDevice::getInstance(), SIGNAL(serialPortChanged(QString)), this, SLOT(_devicePortChanged(QString)));
	connect(DynoDevice::getInstance(), SIGNAL(serialErrorChanged(QSerialPort::SerialPortError)), this, SLOT(_devicePortStatusChanged(QSerialPort::SerialPortError)));
	connect(DynoDevice::getInstance(), SIGNAL(gpsStatusChanged(int,int)), this, SLOT(_deviceGpsStatusChanged(int,int)));
	connect(DynoDevice::getInstance(), SIGNAL(speedChanged(double)), this, SLOT(_speedChanged(double)));

	_ui->lbStatus->setText("");

	_updateGuiState();
	_updateProfileLabels();

	DynoDevice::getInstance()->setPort(DynoSettings::getInstance()->gpsPort());
}

WndMain::~WndMain() {
	delete _axisTorque;
	delete _axisRPM;
	delete _axisPower;
	delete _chart;
	delete _chartView;

	delete _ui;
}

DynoRun * WndMain::_newDynoRun() {
	DynoRun * r = new DynoRun(_chart, _axisRPM, _axisPower, _axisTorque);
	r->setVisibleData(_visibleDataSeries());
	r->setPen(DynoChartSeriesPen::getPen(0));
	return r;
}

uchar WndMain::_visibleDataSeries() {
	uchar v = 0;

	if (_ui->actionViewPower->isChecked()) {
		v |= DynoRun::PowerVisible;
	}

	if (_ui->actionViewPowerOnWheels->isChecked()) {
		v |= DynoRun::PowerWheelVisible;
	}

	if (_ui->actionViewPowerOnWheelsRaw->isChecked()) {
		v |= DynoRun::PowerRawVisible;
	}

	if (_ui->actionViewLosses->isChecked()) {
		v |= DynoRun::LossesVisible;
	}

	if (_ui->actionViewLossesRaw->isChecked()) {
		v |= DynoRun::LossesRawVisible;
	}

	if (_ui->actionViewTorque->isChecked()) {
		v |= DynoRun::TorqueVisible;
	}

	return v;
}

void WndMain::_updateRunsInfoBox() {
	_ui->twRuns->setRowCount(0);
	_chartView->removeRunInfo();

	if (_currentRun) {
		_updateRunInfo(0, _currentRun);
		_chartView->addRunInfo(_currentRun, 0);
	}

	for(int i = 0; i < _runs.count(); i++) {
		_chartView->addRunInfo(_runs[i], i + 1);
		_updateRunInfo(i + 1, _runs[i]);
	}
}

void WndMain::_updateRunInfo(int row, DynoRun * run) {
	QTableWidgetItem * twiName;
	QTableWidgetItem * twiPwrMax;
	QTableWidgetItem * twiTrqMax;
	QTableWidgetItem * twiRPMMax;
	QTableWidgetItem * twiVMax;

	if (_ui->twRuns->rowCount() <= row) {
		_ui->twRuns->setRowCount(row + 1);

		twiName = new QTableWidgetItem();
		_ui->twRuns->setItem(row, 0, twiName);

		twiPwrMax = new QTableWidgetItem();
		_ui->twRuns->setItem(row, 1, twiPwrMax);

		twiTrqMax = new QTableWidgetItem();
		_ui->twRuns->setItem(row, 2, twiTrqMax);

		twiRPMMax = new QTableWidgetItem();
		_ui->twRuns->setItem(row, 3, twiRPMMax);

		twiVMax = new QTableWidgetItem();
		_ui->twRuns->setItem(row, 4, twiVMax);
	}
	else {
		twiName = _ui->twRuns->item(row, 0);
		twiPwrMax = _ui->twRuns->item(row, 1);
		twiTrqMax = _ui->twRuns->item(row, 2);
		twiRPMMax = _ui->twRuns->item(row, 3);
		twiVMax = _ui->twRuns->item(row, 4);
	}

	twiName->setText(run->name());
	twiPwrMax->setText(tr("%1KM @ %2RPM").arg(run->powerMax(), 0, 'f', 1).arg(run->powerMaxRPM()));
	twiTrqMax->setText(tr("%1Nm @ %2RPM").arg(run->torqueMax(), 0, 'f', 1).arg(run->torqueMaxRPM()));
	twiRPMMax->setText(tr("%1RPM").arg(run->rpmMax()));
	twiVMax->setText(tr("%1km/h").arg(run->speedMax(), 0, 'f', 1));

}

void WndMain::_updateGuiState() {
	if (_runInPogress) {
		_ui->menuHelp->setEnabled(false);
		_ui->menuSettings->setEnabled(false);
		_ui->menuView->setEnabled(false);
		_ui->bntLoadRun->setEnabled(false);
		_ui->bntRemoveRun->setEnabled(false);

		_ui->actionRunLoad->setEnabled(false);
		_ui->actionRunSave->setEnabled(false);

		_ui->actionRunStart->setText(tr("&Cancel"));
		_ui->actionRunStart->setShortcut(QKeySequence(Qt::Key_Escape));
	}
	else {
		_ui->menuHelp->setEnabled(true);
		_ui->menuSettings->setEnabled(true);
		_ui->menuView->setEnabled(true);
		_ui->bntLoadRun->setEnabled(true);
		_ui->bntRemoveRun->setEnabled(true);

		_ui->actionRunLoad->setEnabled(true);
		_ui->actionRunSave->setEnabled(true);

		_ui->actionRunStart->setText(tr("S&tart"));
		_ui->actionRunStart->setShortcut(QKeySequence(Qt::Key_Space));
	}
}

void WndMain::_updateProfileLabels() {
	_ui->lbWeight->setText(QString("%1 kg").arg(_currentProfile.weight));
	_ui->lbRPMRatio->setText(QString("%1").arg(_currentProfile.rpmRatio, 0, 'f', 6));
	_ui->lbTemperature->setText(QString("%1 °C").arg(_currentProfile.airTemp, 0, 'f', 1));
	_ui->lbPressure->setText(QString("%1 hPa").arg(_currentProfile.airPress));
}

void WndMain::_updateAxesMax() {
	int maxRpm = 3500, maxPwr = 100, maxTrq = 100;

	if (_currentRun) {
		if (_currentRun->rpmMax() >= maxRpm) {
			maxRpm = _currentRun->rpmMax();
		}

		if (_currentRun->powerMax() >= maxPwr) {
			maxPwr = _currentRun->powerMax();
		}

		if (_currentRun->torqueMax() >= maxTrq) {
			maxTrq = _currentRun->torqueMax();
		}
	}

	for (int i = 0; i < _runs.size(); i++) {
		DynoRun * r = _runs[i];

		if (r->rpmMax() >= maxRpm) {
			maxRpm = r->rpmMax();
		}

		if (r->powerMax() >= maxPwr) {
			maxPwr = r->powerMax();
		}

		if (r->torqueMax() >= maxTrq) {
			maxTrq = r->torqueMax();
		}
	}

	_axisRPM->setMax((maxRpm / 500) * 500 + 500);
	_axisPower->setMax(maxPwr + 40);
	_axisTorque->setMax(maxTrq + 40);
}

void WndMain::_loadCurrentRun() {
	QFileDialog w(this);
	DynoRun * run;

	w.setDirectory(_prevSavePath);
	w.setViewMode(QFileDialog::Detail);
	w.setNameFilter("Dyno results (*.dyno)");
	w.setFileMode(QFileDialog::ExistingFile);

	if (w.exec() != QDialog::Accepted) {
		return;
	}

	_prevSavePath = w.directory().absolutePath();

	run = _newDynoRun();
	run->loadFromFile(w.selectedFiles().at(0));
	run->setName("Run 1");
	if (_currentRun) {
		delete _currentRun;
	}

	_currentRun = run;

	_updateRunsInfoBox();
	_updateAxesMax();
}

void WndMain::_saveCurrentRun() {
	QFileDialog w(this);
	if (_currentRun) {
		w.setDirectory(_prevSavePath);
		w.setViewMode(QFileDialog::Detail);
		w.setNameFilter("Dyno results (*.dyno)");
		w.setFileMode(QFileDialog::AnyFile);
		w.setAcceptMode(QFileDialog::AcceptSave);
		w.setDefaultSuffix("dyno");
		w.selectFile(QString("run_%1.dyno").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")));
		if (w.exec() == QDialog::Accepted) {
			_prevSavePath = w.directory().absolutePath();
			_currentRun->saveToFile(w.selectedFiles().at(0));
		}
	}
}

void WndMain::_exportImage() {
	QFileDialog w(this);

	w.setDirectory(_prevSavePath);
	w.setViewMode(QFileDialog::Detail);
	w.setNameFilter("PNG Images (*.png)");
	w.setFileMode(QFileDialog::AnyFile);
	w.setAcceptMode(QFileDialog::AcceptSave);
	w.setDefaultSuffix("png");
	w.selectFile(QString("export_%1.png").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")));

	if (w.exec() == QDialog::Accepted) {
		QPixmap buff(1280, 720);
		buff.fill(Qt::transparent);

		QPainter painter(&buff);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setPen(QColor(255,34,255,255));

		/* Brzydki hak żeby na szybko eksportować w takim rozmiarze jakchcemy */
		QSize oldSize = _chartView->size();
		_chartView->resize(buff.size());
		_chartView->render(&painter);
		_chartView->resize(oldSize);

		/* Obsługa znaku wodnego */
		QString watermarkPath = DynoSettings::getInstance()->watermarkPath();
		if (!watermarkPath.isEmpty()) {
			QPixmap watermark;
			if (watermark.load(watermarkPath)) {
				painter.setOpacity(0.15);
				painter.drawPixmap(buff.width() / 2 - watermark.width() / 2, buff.height() / 2 - watermark.height() / 2, watermark);
				painter.setOpacity(1);
			}
			else {
				QMessageBox::critical(this, tr("Error"), tr("Unable to load watermark file!"));
				return;
			}
		}

		buff.save(w.selectedFiles().at(0));
	}
}

void WndMain::_loadOtherRun() {
	int count = _runs.count();
	if (count >= 2) {
		QMessageBox::warning(this, tr("Information"), tr("Maxmimum supported runs is 3 (current + 2 other), remove one run first"));
		return;
	}

	QFileDialog w(this);
	DynoRun * run;

	w.setDirectory(_prevSavePath);
	w.setViewMode(QFileDialog::Detail);
	w.setNameFilter("Dyno results (*.dyno)");
	w.setFileMode(QFileDialog::ExistingFile);

	if (w.exec() != QDialog::Accepted) {
		return;
	}

	_prevSavePath = w.directory().absolutePath();

	run = _newDynoRun();
	run->setPen(DynoChartSeriesPen::getPen(count + 1));
	run->loadFromFile(w.selectedFiles().at(0));
	run->setName(QString("Run %1").arg(count + 2));

	_runs.append(run);

	_updateRunsInfoBox();
	_updateAxesMax();
}

void WndMain::_removeRun() {
	DynoRun * run;
	if (_runs.count() > 0) {
		run = _runs.last();
		_runs.removeLast();
		delete run;

		_updateRunsInfoBox();
		_updateAxesMax();
	}
}

void WndMain::_startStopRun() {
	if (_runInPogress) {
		_currentRun->cancelRun();
	}
	else {
		if (_currentRun) {
			delete _currentRun;
		}

		_currentRun = _newDynoRun();
		_currentRun->setParameters(_currentProfile.rpmRatio, _currentProfile.weight);
		connect(_currentRun, SIGNAL(runStateChanged(DynoRun::DynoRunState)), this, SLOT(_runStateChanged(DynoRun::DynoRunState)));
		_runInPogress = true;
		_currentRun->startRun();
	}

	_updateGuiState();
}

void WndMain::_setVechicleProfile() {
	WndProfile wndProfile(_currentProfile, this);
	if (wndProfile.exec() == QDialog::Accepted) {
		_currentProfile = wndProfile.getValue();
		DynoSettings::getInstance()->setLastVehicleProfile(_currentProfile);
		_updateProfileLabels();
		if (_currentRun) {
			_currentRun->setParameters(_currentProfile.rpmRatio, _currentProfile.weight);
			_updateRunsInfoBox();
			_updateAxesMax();
		}
	}
}

void WndMain::_dynoSettings() {
	WndSettings w(this);
	if (w.exec() == QDialog::Accepted) {
		DynoSettings::getInstance()->setDataDir(w.dataDir());
		DynoSettings::getInstance()->setGpsPort(w.gpsPort());
		DynoSettings::getInstance()->setLossTime(w.lossTime());
		DynoSettings::getInstance()->setUserInfo(w.userInfo());
		DynoSettings::getInstance()->setWatermark(w.watermarkFile());

		DynoDevice::getInstance()->setPort(w.gpsPort());		
		_chartView->updateUserInfo(w.userInfo());
	}
}

void WndMain::_about() {
	WndAbout w;
	w.exec();
}

void WndMain::_visiblaDataToogled() {
	uchar v = _visibleDataSeries();

	if (_currentRun) {
		_currentRun->setVisibleData(v);
	}

	for(int i = 0; i < _runs.count(); i++) {
		_runs[i]->setVisibleData(v);
	}
}

void WndMain::_runStateChanged(DynoRun::DynoRunState state) {
	switch (state) {
		case DynoRun::RunInitial: {
			_ui->lbStatus->setText(tr("Waiting for run start..."));
			_updateAxesMax();
			break;
		};
		case DynoRun::RunWaitForSpeed: {
			_ui->lbStatus->setText(tr("Waiting for a speed of 30 km/h to start the run..."));
			break;
		}
		case DynoRun::RunCountdown: {
			_ui->lbStatus->setText(tr("Accelerate, run will start in 3, 2, 1..."));
			break;
		}
		case DynoRun::RunAccelerating: {
			_ui->lbStatus->setText(tr("Accelerate and press clutch at full rpm..."));
			break;
		}
		case DynoRun::RunLosses: {
			_ui->lbStatus->setText(tr("Calculating losses, slow down without braking..."));
			break;
		}
		case DynoRun::RunFinished: {
			_ui->lbStatus->setText(tr("Run finished."));
			_updateRunsInfoBox();
			_updateAxesMax();
			_runInPogress = false;
			break;
		}
		case DynoRun::RunCanceled: {
			_ui->lbStatus->setText(tr("Run canceled."));
			_updateRunsInfoBox();
			_updateAxesMax();
			_runInPogress = false;
			break;
		}
	}
	_updateGuiState();
}

void WndMain::_devicePortChanged(QString name) {
	_ui->lbDynoPort->setText(name);
}

void WndMain::_deviceGpsStatusChanged(int fix, int siv) {
	QString str;

	switch(fix) {
		case DynoDevice::NoFix: str = tr("No fix"); break;
		case DynoDevice::Fix2D: str = tr("2D fix"); break;
		case DynoDevice::Fix3D: str = tr("3D fix"); break;
		default: str = tr("Unknown");
	}

	_ui->lbGpsFix->setText(str);
	_ui->lbSIV->setText(QString("%1").arg(siv));
}

void WndMain::_devicePortStatusChanged(QSerialPort::SerialPortError error) {
	QString str;

	switch(error) {
		case QSerialPort::NoError: str = tr("Ready"); break;
		case QSerialPort::DeviceNotFoundError: str = tr("Device not found"); break;
		case QSerialPort::PermissionError: str = tr("Permission denied"); break;
		case QSerialPort::OpenError: str = tr("Unable to open"); break;
		case QSerialPort::ReadError: str = tr("Read error"); break;
		default: str = tr("Unknown error");
	}

	_ui->lbDynoStatus->setText(str);
}

void WndMain::_speedChanged(double speed) {
	_ui->lbSpeed->setText(QString("%1km/h").arg(speed, 0, 'f', 1));
}

