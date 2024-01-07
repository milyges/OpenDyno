#include "wndmain.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);	
	QCoreApplication::setOrganizationName("milyges");
	QCoreApplication::setOrganizationDomain("mhroczny.net");
	QCoreApplication::setApplicationName("OpenDyno");
	QCoreApplication::setApplicationVersion("dev");
	app.setWindowIcon(QIcon(":/icons/icon.png"));
	WndMain wndMain;
	wndMain.show();
	return app.exec();
}
