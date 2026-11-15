#include "Screenshots.h"
#include "gui/MainWindow.h"
#include "gui/PatientForm.h"
#include <QApplication>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>

using namespace scapms;

namespace {

void settle() {
	QApplication::processEvents();
	QApplication::processEvents();
}

void press(QWidget *scope, const QString &text) {
	for (auto *button : scope->findChildren<QPushButton *>()) {
		if (button->text() == text) {
			button->click();
			return;
		}
	}
}

void queuePatients(MainWindow &window, const QList<int> &positions) {
	auto *panel = window.findChild<QueuePanel *>();
	auto *picker = panel->findChild<PatientPicker *>();
	for (int position : positions) {
		picker->setCurrentIndex(position);
		press(panel, "Add to queue");
	}
	settle();
}

void showTab(MainWindow &window, QWidget *panel) {
	window.findChild<QTabWidget *>()->setCurrentWidget(panel);
	settle();
}

void saveWindow(MainWindow &window, const QString &path) {
	settle();
	window.grab().save(path);
}

void saveForm(PatientForm &form, const QString &path) {
	form.show();
	settle();
	form.grab().save(path);
}

void fillForm(PatientForm &form, const QString &category, const QString &name, const QString &age, const QString &contact) {
	auto fields = form.findChildren<QLineEdit *>(QString(), Qt::FindDirectChildrenOnly);
	form.findChild<QComboBox *>()->setCurrentText(category);
	fields[1]->setText(name);
	fields[2]->setText(age);
	fields[3]->setText(contact);
}

}

void captureScreenshots(const QString &imageDirectory) {
	MainWindow window;
	window.resize(1180, 760);
	window.show();
	QMetaObject::invokeMethod(&window, "enterClinic");
	settle();

	auto *search = window.findChild<SearchPanel *>();
	auto *mode = search->findChildren<QComboBox *>()[0];
	auto *searchEdit = search->findChild<QLineEdit *>();
	queuePatients(window, {6, 7, 1});
	mode->setCurrentIndex(1);
	searchEdit->setText("a");
	window.findChild<DashboardPanel *>()->refresh();
	showTab(window, window.findChild<DashboardPanel *>());
	saveWindow(window, imageDirectory + "/01-dashboard.png");

	showTab(window, window.findChild<PatientPanel *>());
	saveWindow(window, imageDirectory + "/03-patient-directory.png");

	showTab(window, window.findChild<AppointmentPanel *>());
	saveWindow(window, imageDirectory + "/04-appointments.png");

	showTab(window, window.findChild<QueuePanel *>());
	saveWindow(window, imageDirectory + "/06-waiting-queue.png");

	showTab(window, search);
	auto boxes = search->findChildren<QComboBox *>();
	boxes[1]->setCurrentIndex(1);
	boxes[2]->setCurrentIndex(0);
	press(search, "SORT");
	mode->setCurrentIndex(1);
	searchEdit->setText("Chidi Okeke");
	saveWindow(window, imageDirectory + "/05-search-and-sort.png");

	showTab(window, window.findChild<ReportsPanel *>());
	saveWindow(window, imageDirectory + "/07-reports.png");

	auto taken = [](int) { return false; };
	PatientForm registration(false, PatientDraft(), taken);
	fillForm(registration, "FollowUp", "Ada Nnamdi", "29", "0816-555-0113");
	saveForm(registration, imageDirectory + "/02-registration.png");

	PatientForm invalid(false, PatientDraft(), taken);
	fillForm(invalid, "Emergency", "Ada Nnamdi", "150", "0816-555-0113");
	saveForm(invalid, imageDirectory + "/02b-registration-validation.png");
}
