#include "gui/DashboardPanel.h"
#include "gui/Theme.h"
#include "gui/Widgets.h"
#include "utils/Constants.h"
#include <QDate>
#include <QFrame>
#include <QGridLayout>
#include <QVBoxLayout>
#include <utility>

namespace scapms {

namespace {

QFrame *makeCard(const QString &caption, QLabel *&value) {
	auto *card = new QFrame;
	card->setObjectName("card");
	card->setStyleSheet(theme::CARD_STYLE);
	auto *layout = new QVBoxLayout(card);
	value = new QLabel("0");
	value->setStyleSheet("font-size:30px;font-weight:700;color:#102a43;background:transparent");
	auto *label = new QLabel(caption);
	label->setStyleSheet("color:#486581;background:transparent");
	layout->addWidget(value);
	layout->addWidget(label);
	return card;
}

std::string isoDate(const QDate &date) {
	return date.toString("yyyy-MM-dd").toStdString();
}

}

DashboardPanel::DashboardPanel(ClinicManager &manager, QWidget *parent) : Panel(manager, parent) {
	auto *root = new QVBoxLayout(this);
	root->addWidget(makeHeading("Dashboard"));
	auto *cards = new QGridLayout;
	cards->addWidget(makeCard("Registered patients", patientsValue), 0, 0);
	cards->addWidget(makeCard("Waiting in queue", waitingValue), 0, 1);
	cards->addWidget(makeCard("Pending appointments", pendingValue), 0, 2);
	cards->addWidget(makeCard("Appointments today", todayValue), 0, 3);
	cards->addWidget(makeCard("Undo steps available", undoValue), 0, 4);
	root->addLayout(cards);

	auto *lower = new QGridLayout;
	auto *recentBox = new QVBoxLayout;
	recentBox->addWidget(makeHeading("Frequently accessed patients"));
	recentBox->addWidget(makeHint("The 10 most recently viewed, searched or scheduled patients."));
	recentTable = new PatientTable;
	recentBox->addWidget(recentTable);
	auto *reminderBox = new QVBoxLayout;
	reminderBox->addWidget(makeHeading("Appointment reminders"));
	reminderBox->addWidget(makeHint("Pending appointments scheduled for today or tomorrow."));
	reminderList = new QListWidget;
	reminderBox->addWidget(reminderList);
	lower->addLayout(recentBox, 0, 0);
	lower->addLayout(reminderBox, 0, 1);
	lower->setColumnStretch(0, 3);
	lower->setColumnStretch(1, 2);
	root->addLayout(lower, 1);

	refreshTimer = new QTimer(this);
	refreshTimer->setInterval(constants::DASHBOARD_REFRESH_MS);
	connect(refreshTimer, &QTimer::timeout, this, &DashboardPanel::refresh);
	refreshTimer->start();
	reminderTimer = new QTimer(this);
	reminderTimer->setInterval(constants::REMINDER_CHECK_MS);
	connect(reminderTimer, &QTimer::timeout, this, &DashboardPanel::checkReminders);
	reminderTimer->start();
}

void DashboardPanel::refresh() {
	const auto &controller = manager.appointmentController();
	patientsValue->setText(QString::number(manager.patients().size()));
	waitingValue->setText(QString::number(manager.queuedPatients().size()));
	pendingValue->setText(QString::number(controller.summary().pending));
	todayValue->setText(QString::number(controller.pendingOn(isoDate(QDate::currentDate())).size()));
	undoValue->setText(QString::number(manager.undoDepth()));
	recentTable->setPatients(manager.recentPatients());

	reminderList->clear();
	QDate today = QDate::currentDate();
	const std::pair<QDate, QString> days[] = {{today, "Today"}, {today.addDays(1), "Tomorrow"}};
	for (const auto &day : days) {
		for (const auto &entry : controller.pendingOn(isoDate(day.first))) {
			reminderList->addItem(QString("[%1] %2 - appointment #%3 on %4")
				.arg(day.second, QString::fromStdString(entry.patient->name()))
				.arg(entry.appointment->id())
				.arg(QString::fromStdString(entry.appointment->date())));
		}
	}
	if (reminderList->count() == 0) {
		reminderList->addItem("No reminders.");
	}
}

void DashboardPanel::checkReminders() {
	const auto &controller = manager.appointmentController();
	QDate today = QDate::currentDate();
	for (const auto &date : {today, today.addDays(1)}) {
		for (const auto &entry : controller.pendingOn(isoDate(date))) {
			int id = entry.appointment->id();
			if (announced.contains(id)) {
				continue;
			}
			announced.insert(id);
			QString name = QString::fromStdString(entry.patient->name());
			QString when = QString::fromStdString(entry.appointment->date());
			emit notified(QString("Reminder: %1 has an appointment on %2.").arg(name, when));
		}
	}
	refresh();
}

}
