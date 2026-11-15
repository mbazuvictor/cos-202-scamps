#include "gui/PatientPanel.h"
#include "gui/PatientForm.h"
#include "gui/Theme.h"
#include "gui/Widgets.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>

namespace scapms {

PatientPanel::PatientPanel(ClinicManager &manager, QWidget *parent) : Panel(manager, parent) {
	auto *root = new QVBoxLayout(this);
	root->addWidget(makeHeading("Patient directory"));
	root->addWidget(makeHint("Select a patient to edit or delete. Double-click a row to edit it."));
	auto *actions = new QHBoxLayout;
	auto *addButton = makeButton("+ Add patient", "Register a new patient (Ctrl+Shift+N)");
	auto *editButton = makeButton("Edit selected", "Change the selected patient's details");
	auto *deleteButton = makeButton("Delete selected", "Delete the selected patient. Edit > Undo restores it.");
	deleteButton->setStyleSheet(theme::DELETE_BUTTON_STYLE);
	actions->addWidget(addButton);
	actions->addWidget(editButton);
	actions->addWidget(deleteButton);
	actions->addStretch();
	root->addLayout(actions);
	table = new PatientTable;
	root->addWidget(table);
	connect(addButton, &QPushButton::clicked, this, &PatientPanel::addPatient);
	connect(editButton, &QPushButton::clicked, this, &PatientPanel::editPatient);
	connect(deleteButton, &QPushButton::clicked, this, &PatientPanel::deletePatient);
	connect(table, &QTableWidget::cellDoubleClicked, this, &PatientPanel::editPatient);
}

void PatientPanel::refresh() {
	table->setPatients(manager.patients());
}

void PatientPanel::addPatient() {
	auto taken = [this](int id) {
		for (const auto *patient : manager.patients()) {
			if (patient->id() == id) {
				return true;
			}
		}
		return false;
	};
	PatientForm form(false, PatientDraft(), taken, this);
	if (form.exec() != QDialog::Accepted) {
		return;
	}
	PatientDraft draft = form.draft();
	run([&] {
		int requestedId = draft.id.isEmpty() ? 0 : draft.id.toInt();
		int id = manager.addPatient(draft.type.toStdString(), draft.toDetails(), requestedId);
		return QString("Patient %1 registered successfully.").arg(id);
	});
}

void PatientPanel::editPatient() {
	int id = table->selectedPatientId();
	if (id < 0) {
		emit failed("Select a patient before choosing Edit.");
		return;
	}
	PatientDraft draft;
	guard([&] {
		const PatientRecord &patient = manager.patientById(id);
		draft.type = QString::fromStdString(patient.category());
		draft.name = QString::fromStdString(patient.name());
		draft.age = QString::number(patient.age());
		draft.contact = QString::fromStdString(patient.contact());
		draft.detail = QString::fromStdString(patient.detail());
	});
	PatientForm form(true, draft, [](int) { return false; }, this);
	if (form.exec() != QDialog::Accepted) {
		return;
	}
	PatientDraft edited = form.draft();
	run([&] {
		manager.editPatient(id, edited.toDetails());
		return QString("Patient updated.");
	});
}

void PatientPanel::deletePatient() {
	int id = table->selectedPatientId();
	if (id < 0) {
		emit failed("Select a patient before choosing Delete.");
		return;
	}
	QString name = table->item(table->currentRow(), 1)->text();
	if (QMessageBox::question(this, "Delete patient", "Delete " + name + "? This can be undone.") != QMessageBox::Yes) {
		return;
	}
	run([&] {
		manager.deletePatient(id);
		return QString("Patient deleted. Use Edit > Undo to restore it.");
	});
}

}
