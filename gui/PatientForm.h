#pragma once

#include "model/PatientDetails.h"
#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <functional>

namespace scapms {

struct PatientDraft {
	QString id;
	QString type = "General";
	QString name;
	QString age = "0";
	QString contact;
	QString detail;

	PatientDetails toDetails() const {
		return {name.toStdString(), age.toInt(), contact.toStdString(), detail.toStdString()};
	}
};

class PatientForm : public QDialog {
	Q_OBJECT

public:
	PatientForm(bool editing, const PatientDraft &initial, std::function<bool(int)> isIdTaken,
		QWidget *parent = nullptr);
	PatientDraft draft() const;

private slots:
	void adaptToCategory();
	void validateInputs();

private:
	QString findError() const;
	QString detailText() const;

	bool editing;
	bool touched = false;
	std::function<bool(int)> isIdTaken;
	QLabel *idLabel;
	QLabel *detailLabel;
	QLabel *dateLabel;
	QLineEdit *idField;
	QLineEdit *nameField;
	QLineEdit *ageField;
	QLineEdit *contactField;
	QComboBox *typeField;
	QLineEdit *detailField;
	QDateEdit *dateField;
	QLabel *errorLabel;
	QDialogButtonBox *buttons;
};

}
