#include "gui/PatientForm.h"
#include "gui/Theme.h"
#include "model/PatientFactory.h"
#include "model/PatientRecord.h"
#include "utils/Constants.h"
#include <QFormLayout>
#include <QPushButton>

namespace scapms {

PatientForm::PatientForm(bool editing, const PatientDraft &initial, std::function<bool(int)> isIdTaken,
	QWidget *parent)
	: QDialog(parent), editing(editing), isIdTaken(std::move(isIdTaken)) {
	setWindowTitle(editing ? "Edit patient record" : "Register new patient");
	setMinimumSize(560, 380);
	auto *layout = new QFormLayout(this);
	layout->setLabelAlignment(Qt::AlignRight);
	layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	layout->setHorizontalSpacing(18);
	layout->setVerticalSpacing(14);

	idLabel = new QLabel("Patient ID (optional)");
	idField = new QLineEdit(initial.id);
	idField->setPlaceholderText("Leave blank to generate an ID automatically");
	idField->setToolTip("Optional. Enter a unique number from 1 to 999999, or leave blank.");
	nameField = new QLineEdit(initial.name);
	nameField->setPlaceholderText("Enter the patient's full name");
	nameField->setToolTip("The patient's full name (commas and quotes are not allowed).");
	ageField = new QLineEdit(initial.age);
	ageField->setPlaceholderText("0 - 120");
	ageField->setToolTip("Whole number between 0 and 120.");
	contactField = new QLineEdit(initial.contact);
	contactField->setPlaceholderText("Phone number or email address");
	contactField->setToolTip("How the clinic can reach the patient.");
	typeField = new QComboBox;
	for (const auto &category : PatientFactory::categories()) {
		typeField->addItem(QString::fromStdString(category));
	}
	typeField->setCurrentText(initial.type);
	typeField->setEnabled(!editing);
	typeField->setToolTip(editing ? "The category cannot be changed after registration."
								  : "Choosing a category changes the extra fields below.");

	detailLabel = new QLabel;
	detailField = new QLineEdit(initial.detail);
	dateLabel = new QLabel("Previous visit date");
	dateField = new QDateEdit;
	dateField->setDisplayFormat("yyyy-MM-dd");
	dateField->setCalendarPopup(true);
	QDate previous = QDate::fromString(initial.detail, "yyyy-MM-dd");
	dateField->setDate(previous.isValid() ? previous : QDate::currentDate());
	dateField->setToolTip("The date of the visit this follow-up relates to.");

	layout->addRow(idLabel, idField);
	layout->addRow("Full name", nameField);
	layout->addRow("Age", ageField);
	layout->addRow("Phone / contact", contactField);
	layout->addRow("Patient category", typeField);
	layout->addRow(detailLabel, detailField);
	layout->addRow(dateLabel, dateField);

	errorLabel = new QLabel;
	errorLabel->setStyleSheet(theme::ERROR_STYLE);
	errorLabel->setWordWrap(true);
	layout->addRow(errorLabel);
	buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
	layout->addRow(buttons);

	idLabel->setVisible(!editing);
	idField->setVisible(!editing);

	connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(typeField, &QComboBox::currentTextChanged, this, &PatientForm::adaptToCategory);
	for (auto *field : {idField, nameField, ageField, contactField, detailField}) {
		connect(field, &QLineEdit::textChanged, this, &PatientForm::validateInputs);
	}
	connect(dateField, &QDateEdit::dateChanged, this, &PatientForm::validateInputs);
	adaptToCategory();
}

void PatientForm::adaptToCategory() {
	bool followUp = typeField->currentText() == constants::CATEGORY_FOLLOW_UP;
	detailLabel->setVisible(!followUp);
	detailField->setVisible(!followUp);
	dateLabel->setVisible(followUp);
	dateField->setVisible(followUp);
	if (!followUp) {
		bool emergency = typeField->currentText() == constants::CATEGORY_EMERGENCY;
		detailLabel->setText(emergency ? "Emergency contact / condition" : "Reason for visit");
		detailField->setPlaceholderText(emergency ? "Who to call, or the presenting condition"
										: "Why the patient is visiting");
	}
	validateInputs();
}

QString PatientForm::detailText() const {
	if (typeField->currentText() == constants::CATEGORY_FOLLOW_UP) {
		return dateField->date().toString("yyyy-MM-dd");
	}
	return detailField->text().trimmed();
}

QString PatientForm::findError() const {
	try {
		QString idText = idField->text().trimmed();
		if (!editing && !idText.isEmpty()) {
			bool ok = false;
			int id = idText.toInt(&ok);
			if (!ok) {
				return "Error: Patient ID must be a number.";
			}
			PatientRecord::validateId(id);
			if (isIdTaken(id)) {
				return "Error: Patient ID already exists.";
			}
		}
		bool ageOk = false;
		int age = ageField->text().trimmed().toInt(&ageOk);
		if (!ageOk) {
			return "Error: Patient age must be a whole number between 0 and 120.";
		}
		PatientDetails details{nameField->text().trimmed().toStdString(), age,
			contactField->text().trimmed().toStdString(), detailText().toStdString()};
		PatientFactory::validate(typeField->currentText().toStdString(), details);
	} catch (const std::exception &error) {
		return error.what();
	}
	return "";
}

void PatientForm::validateInputs() {
	touched = touched || sender() != nullptr;
	QString error = findError();
	errorLabel->setText(touched ? error : "");
	buttons->button(QDialogButtonBox::Save)->setEnabled(error.isEmpty());
}

PatientDraft PatientForm::draft() const {
	PatientDraft result;
	result.id = idField->text().trimmed();
	result.type = typeField->currentText();
	result.name = nameField->text().trimmed();
	result.age = ageField->text().trimmed();
	result.contact = contactField->text().trimmed();
	result.detail = detailText();
	return result;
}

}
