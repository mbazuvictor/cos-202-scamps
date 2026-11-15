#include "gui/MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	QApplication::setApplicationName("SCAPMS");
	QApplication::setOrganizationName("COS202");
	scapms::MainWindow window;
	window.show();
	return app.exec();
}
