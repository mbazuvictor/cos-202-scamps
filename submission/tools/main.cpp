#include "Diagrams.h"
#include "SampleData.h"
#include "Screenshots.h"
#include <QApplication>
#include <QDir>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	if (argc < 2) {
		return 1;
	}
	QString root = QDir(argv[1]).absolutePath();
	QString output = root + "/submission";
	QString images = output + "/images";
	QString work = output + "/work";
	QDir().mkpath(images);
	seedSampleData(work, output + "/sample-data.csv");
	captureScreenshots(images);
	drawDiagrams(images);
	return 0;
}
