#include "gui/PatientPicker.h"
#include <QCompleter>
#include <QLineEdit>

namespace scapms {

PatientPicker::PatientPicker(QWidget *parent) : QComboBox(parent) {
	setEditable(true);
	setInsertPolicy(QComboBox::NoInsert);
	setMinimumWidth(360);
	setToolTip("Click the arrow or type part of a name, ID or contact to find a patient.");
}

void PatientPicker::setPatients(const PatientList &patients) {
	int previous = selectedPatientId();
	clear();
	if (patients.empty()) {
		addItem("No patients yet - add a patient first", -1);
		setEnabled(false);
		return;
	}
	setEnabled(true);
	int restore = -1;
	for (const auto *patient : patients) {
		QString name = QString::fromStdString(patient->name());
		QString contact = QString::fromStdString(patient->contact());
		QString label = QString("%1 - %2 (%3)").arg(patient->id()).arg(name, contact);
		addItem(label, patient->id());
		if (patient->id() == previous) {
			restore = count() - 1;
		}
	}
	setCurrentIndex(restore);
	lineEdit()->setPlaceholderText("Click the arrow or type to search patients");
	auto *finder = new QCompleter(model(), this);
	finder->setCaseSensitivity(Qt::CaseInsensitive);
	finder->setFilterMode(Qt::MatchContains);
	setCompleter(finder);
}

int PatientPicker::selectedPatientId() const {
	if (!isEnabled()) {
		return -1;
	}
	int id = currentData().toInt();
	if (id > 0) {
		return id;
	}
	QString typed = currentText().trimmed();
	for (int i = 0; i < count(); i++) {
		if (itemText(i).compare(typed, Qt::CaseInsensitive) == 0) {
			return itemData(i).toInt();
		}
	}
	return -1;
}

}
