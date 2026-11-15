#include "gui/QueuePanel.h"
#include "gui/Theme.h"
#include "gui/Widgets.h"
#include "utils/Constants.h"
#include "utils/Exceptions.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>

namespace scapms {

QueuePanel::QueuePanel(ClinicManager &manager, QWidget *parent) : Panel(manager, parent) {
	auto *root = new QVBoxLayout(this);
	root->addWidget(makeHeading("Waiting queue"));
	root->addWidget(makeHint("Add an existing patient to the queue. The first patient added is called first."));
	auto *controls = new QHBoxLayout;
	picker = new PatientPicker;
	auto *addButton = makeButton("Add to queue", "Place the chosen patient at the back of the queue");
	auto *callButton = makeButton("Call next patient", "Call the patient who has waited longest");
	controls->addWidget(picker);
	controls->addWidget(addButton);
	controls->addWidget(callButton);
	root->addLayout(controls);

	auto *alertRow = new QHBoxLayout;
	alertMinutes = new QSpinBox;
	alertMinutes->setRange(1, 240);
	alertMinutes->setValue(constants::DEFAULT_WAIT_ALERT_MINUTES);
	alertMinutes->setSuffix(" min");
	alertMinutes->setToolTip("Alert staff when the first patient in the queue has waited this long");
	alertRow->addWidget(new QLabel("Alert after"));
	alertRow->addWidget(alertMinutes);
	alertRow->addStretch();
	root->addLayout(alertRow);
	waitLabel = new QLabel;
	root->addWidget(waitLabel);

	table = new PatientTable;
	table->verticalHeader()->setVisible(true);
	root->addWidget(table);

	timer = new QTimer(this);
	timer->setInterval(constants::QUEUE_TICK_MS);
	connect(timer, &QTimer::timeout, this, &QueuePanel::updateWaitStatus);
	timer->start();
	connect(addButton, &QPushButton::clicked, this, &QueuePanel::enqueuePatient);
	connect(callButton, &QPushButton::clicked, this, &QueuePanel::callNext);
	connect(alertMinutes, QOverload<int>::of(&QSpinBox::valueChanged), this, &QueuePanel::updateWaitStatus);
}

void QueuePanel::refresh() {
	picker->setPatients(manager.patients());
	table->setPatients(manager.queuedPatients());
	updateWaitStatus();
}

void QueuePanel::enqueuePatient() {
	run([&] {
		int id = picker->selectedPatientId();
		if (id < 0) {
			throw ClinicException("Error: Add a patient first, then select them here.");
		}
		manager.enqueue(id);
		return QString("Patient added to the queue.");
	}, false);
}

void QueuePanel::callNext() {
	run([&] { return QString::fromStdString(manager.callNext()); }, false);
}

void QueuePanel::updateWaitStatus() {
	auto wait = manager.longestWait();
	if (!wait) {
		waitLabel->setStyleSheet(theme::HINT_STYLE);
		waitLabel->setText("Nobody is waiting.");
		alertedPatientId = -1;
		return;
	}
	long seconds = wait->seconds;
	QString name = QString::fromStdString(wait->patientName);
	QString clock = QString("%1:%2").arg(seconds / 60).arg(seconds % 60, 2, 10, QChar('0'));
	QString text = QString("Longest wait: %1 (%2)").arg(name, clock);
	bool overdue = seconds >= alertMinutes->value() * 60L;
	waitLabel->setStyleSheet(overdue ? theme::ERROR_STYLE : theme::HINT_STYLE);
	waitLabel->setText(overdue ? "ALERT - " + text : text);
	if (overdue && alertedPatientId != wait->patientId) {
		alertedPatientId = wait->patientId;
		emit notified("Waiting alert: " + text);
	}
}

}
