#pragma once

#include "gui/Panel.h"
#include "gui/PatientTable.h"
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>

namespace scapms {

class SearchPanel : public Panel {
	Q_OBJECT

public:
	explicit SearchPanel(ClinicManager &manager, QWidget *parent = nullptr);
	void refresh() override;
	void focusSearch();

private slots:
	void searchNow();
	void searchWhileTyping();
	void sortNow();
	void explainAlgorithm();

private:
	void showAll();
	void runSearch();
	QString orderText() const;

	QComboBox *searchMode;
	QLineEdit *searchEdit;
	QLabel *searchStatus;
	QComboBox *sortAlgorithm;
	QComboBox *sortField;
	QLabel *orderLabel;
	QLabel *explanation;
	PatientTable *table;
};

}
