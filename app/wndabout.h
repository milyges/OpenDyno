#ifndef WNDABOUT_H
#define WNDABOUT_H

#include <QDialog>

namespace Ui {
	class WndAbout;
}

class WndAbout : public QDialog
{
		Q_OBJECT

	public:
		explicit WndAbout(QWidget *parent = nullptr);
		~WndAbout();

	private:
		Ui::WndAbout *ui;
};

#endif // WNDABOUT_H
