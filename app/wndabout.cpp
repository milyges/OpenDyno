#include "wndabout.h"
#include "ui_wndabout.h"

WndAbout::WndAbout(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::WndAbout)
{
	ui->setupUi(this);
}

WndAbout::~WndAbout()
{
	delete ui;
}
