#include "gui/AppointmentPanel.h"
#include "gui/Theme.h"
#include "gui/Widgets.h"
#include "utils/Constants.h"
#include "utils/Exceptions.h"
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>

namespace scapms {

namespace {

void colorRow(QTableWidgetItem *item, const Appointment &appointment) {
	const char *background = theme::PENDING_BACKGROUND;
	const char *text = theme::PENDING_TEXT;
	if (appointment.isCompleted()) {
		background = theme::COMPLETED_BACKGROUND;
		text = theme::COMPLETED_TEXT;
	} else if (appointment.isCancelled()) {
		background = theme::CANCELLED_BACKGROUND;
		text = theme::CANCELLED_TEXT;
		QFont font = item->font();
		font.setStrikeOut(true);
		item->setFont(font);
	}
	item->setBackground(QBrush(QColor(background)));
	item->setForeground(QBrush(QColor(text)));
}

}

AppointmentPanel::AppointmentPanel(ClinicManager &manager, QWidget *parent) : Panel(manager, parent) {
	auto *root = new QVBoxLayout(this);
	root->addWidget(makeHeading("Appointments"));
	auto *form = new QFormLayout;
	picker = new PatientPicker;
	dateEdit = new QDateEdit(QDate::currentDate());
	dateEdit->setDisplayFormat("dd MMMM yyyy");
	dateEdit->setCalendarPopup(true);
	dateEdit->setMinimumWidth(360);
	dateEdit->setToolTip("Choose day, month, and year from the calendar");
	dateWarning = new QLabel;
	dateWarning->setStyleSheet(theme::WARNING_STYLE);
	form->addRow("Patient", picker);
	form->addRow("Appointment date", dateEdit);
	form->addRow("", dateWarning);
	root->addLayout(form);

	auto *actions = new QHBoxLayout;
	auto *scheduleButton = makeButton("Schedule appointment", "Book the chosen date for the chosen patient");
	auto *cancelButton = makeButton("Cancel selected", "Cancel the selected pending appointment (can be undone)");
	auto *completeButton = makeButton("Complete selected", "Mark the selected pending appointment as completed");
	statusFilter = new QComboBox;
	statusFilter->addItems({"All", constants::STATUS_PENDING, constants::STATUS_COMPLETED,
		constants::STATUS_CANCELLED});
	statusFilter->setToolTip("Show only appointments with this status");
	actions->addWidget(scheduleButton);
	actions->addWidget(cancelButton);
	actions->addWidget(completeButton);
	actions->addStretch();
	actions->addWidget(new QLabel("Show"));
	actions->addWidget(statusFilter);
	root->addLayout(actions);

	table = new QTableWidget;
	table->setColumnCount(6);
	table->setHorizontalHeaderLabels({"Appointment", "Patient ID", "Name", "Category", "Date", "Status"});
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->horizontalHeader()->setStretchLastSection(true);
	table->verticalHeader()->setVisible(false);
	root->addWidget(table);

	connect(scheduleButton, &QPushButton::clicked, this, &AppointmentPanel::scheduleAppointment);
	connect(cancelButton, &QPushButton::clicked, this, &AppointmentPanel::cancelAppointment);
	connect(completeButton, &QPushButton::clicked, this, &AppointmentPanel::completeAppointment);
	connect(dateEdit, &QDateEdit::dateChanged, this, &AppointmentPanel::warnIfPast);
	connect(statusFilter, &QComboBox::currentTextChanged, this, &AppointmentPanel::refresh);
	warnIfPast();
}

void AppointmentPanel::warnIfPast() {
	dateWarning->setText(dateEdit->date() < QDate::currentDate() ? "Warning: this date is in the past." : "");
}

void AppointmentPanel::refresh() {
	picker->setPatients(manager.patients());
	QString filter = statusFilter->currentText();
	auto entries = manager.appointmentController().entries();
	table->setRowCount(0);
	for (const auto &entry : entries) {
		QString status = QString::fromStdString(entry.appointment->status());
		if (filter != "All" && filter != status) {
			continue;
		}
		int row = table->rowCount();
		table->insertRow(row);
		QStringList values = {
			QString::number(entry.appointment->id()),
			QString::number(entry.patient->id()),
			QString::fromStdString(entry.patient->name()),
			QString::fromStdString(entry.patient->category()),
			QString::fromStdString(entry.appointment->date()),
			status,
		};
		for (int column = 0; column < values.size(); column++) {
			auto *item = new QTableWidgetItem(values[column]);
			colorRow(item, *entry.appointment);
			table->setItem(row, column, item);
		}
	}
}

void AppointmentPanel::scheduleAppointment() {
	run([&] {
		int patientId = picker->selectedPatientId();
		if (patientId < 0) {
			throw ClinicException("Error: Add a patient first, then select a patient from the dropdown.");
		}
		int id = manager.scheduleAppointment(patientId, dateEdit->date().toString("yyyy-MM-dd").toStdString());
		return QString("Appointment #%1 scheduled.").arg(id);
	});
}

template <typename Action>
void AppointmentPanel::actOnSelected(const QString &message, Action action) {
	int row = table->currentRow();
	if (row < 0) {
		emit failed("Select an appointment from the table first.");
		return;
	}
	int appointmentId = table->item(row, 0)->text().toInt();
	int patientId = table->item(row, 1)->text().toInt();
	run([&] {
		action(patientId, appointmentId);
		return message;
	});
}

void AppointmentPanel::cancelAppointment() {
	actOnSelected("Appointment cancelled.", [this](int patientId, int appointmentId) {
		manager.cancelAppointment(patientId, appointmentId);
	});
}

void AppointmentPanel::completeAppointment() {
	actOnSelected("Appointment completed.", [this](int patientId, int appointmentId) {
		manager.completeAppointment(patientId, appointmentId);
	});
}

}
