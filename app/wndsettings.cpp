#include "wndsettings.h"
#include "ui_wndsettings.h"
#include "dynosettings.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QFileDialog>

WndSettings::WndSettings(QWidget *parent) : QDialog(parent), _ui(new Ui::WndSettings) {
	int i;
	_ui->setupUi(this);
	QString portName;

	_ui->leDataDir->setText(DynoSettings::getInstance()->dataDir());
	_ui->sbLossTime->setValue(DynoSettings::getInstance()->lossTime());
	_ui->pteUserInfo->setPlainText(DynoSettings::getInstance()->userInfo());
	_ui->leWatermark->setText(DynoSettings::getInstance()->watermarkPath());

	portName = DynoSettings::getInstance()->gpsPort();

	QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts();
	for (i = 0; i < info.size(); i++) {
		_ui->cbGpsPort->addItem(info[i].portName());
		if (portName == info[i].portName()) {
			_ui->cbGpsPort->setCurrentIndex(i);
		}
	}

	connect(_ui->bntBrowse, SIGNAL(released()), this, SLOT(_browseDir()));
	connect(_ui->bntBrowseWatermark, SIGNAL(released()), this, SLOT(_browseWatermark()));
}

WndSettings::~WndSettings() {
	delete _ui;
}

QString WndSettings::dataDir() {
	return _ui->leDataDir->text();
}

QString WndSettings::gpsPort() {
	return _ui->cbGpsPort->currentText();
}

int WndSettings::lossTime() {
	return _ui->sbLossTime->value();
}

QString WndSettings::userInfo() {
	return _ui->pteUserInfo->toPlainText();
}

QString WndSettings::watermarkFile() {
	return _ui->leWatermark->text();
}

void WndSettings::_browseDir() {
	QFileDialog w(this);
	w.setDirectory(_ui->leDataDir->text());
	w.setViewMode(QFileDialog::Detail);
	w.setFileMode(QFileDialog::Directory);

	if (w.exec() != QDialog::Accepted) {
		return;
	}

	_ui->leDataDir->setText(w.selectedFiles().at(0));
}

void WndSettings::_browseWatermark() {
	QFileDialog w(this);

	w.setViewMode(QFileDialog::Detail);
	w.setNameFilter("PNG Images (*.png)");
	w.setFileMode(QFileDialog::AnyFile);
	w.setAcceptMode(QFileDialog::AcceptOpen);
	w.setDefaultSuffix("png");

	if (w.exec() != QDialog::Accepted) {
		return;
	}

	_ui->leWatermark->setText(w.selectedFiles().at(0));
}
