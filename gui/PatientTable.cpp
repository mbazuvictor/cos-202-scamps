#include "gui/PatientTable.h"
#include "gui/Theme.h"
#include "utils/Constants.h"
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QHeaderView>

namespace scapms {

namespace {

struct RowColors {
	QColor background;
	QColor text;
};

RowColors colorsFor(const std::string &category) {
	if (category == constants::CATEGORY_EMERGENCY) {
		return {QColor(theme::EMERGENCY_BACKGROUND), QColor(theme::EMERGENCY_TEXT)};
	}
	if (category == constants::CATEGORY_FOLLOW_UP) {
		return {QColor(theme::FOLLOW_UP_BACKGROUND), QColor(theme::FOLLOW_UP_TEXT)};
	}
	return {QColor(theme::GENERAL_BACKGROUND), QColor(theme::GENERAL_TEXT)};
}

}

PatientTable::PatientTable(QWidget *parent) : QTableWidget(parent) {
	setColumnCount(7);
	setHorizontalHeaderLabels({"ID", "Full name", "Age", "Phone / contact", "Category", "Priority", "Details"});
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);
	horizontalHeader()->setStretchLastSection(true);
	setColumnWidth(0, 80);
	setColumnWidth(1, 200);
	setColumnWidth(2, 60);
	setColumnWidth(3, 200);
	setColumnWidth(4, 110);
	setColumnWidth(5, 130);
	verticalHeader()->setVisible(false);
	setToolTip("Emergency patients are shown in red, follow-ups in blue and general patients in white.");
}

void PatientTable::setPatients(const PatientList &patients) {
	int previous = selectedPatientId();
	setRowCount(0);
	setRowCount(static_cast<int>(patients.size()));
	for (int row = 0; row < static_cast<int>(patients.size()); row++) {
		const PatientRecord *patient = patients[row];
		RowColors colors = colorsFor(patient->category());
		QStringList values = {
			QString::number(patient->id()),
			QString::fromStdString(patient->name()),
			QString::number(patient->age()),
			QString::fromStdString(patient->contact()),
			QString::fromStdString(patient->category()),
			QString::fromStdString(patient->priorityLabel()),
			QString::fromStdString(patient->detailLabel() + ": " + patient->detail()),
		};
		for (int column = 0; column < values.size(); column++) {
			auto *item = new QTableWidgetItem(values[column]);
			item->setBackground(QBrush(colors.background));
			item->setForeground(QBrush(colors.text));
			if (patient->category() == constants::CATEGORY_EMERGENCY) {
				QFont font = item->font();
				font.setBold(true);
				item->setFont(font);
			}
			if (column == 0) {
				item->setData(Qt::UserRole, patient->id());
			}
			setItem(row, column, item);
		}
		if (patient->id() == previous) {
			selectRow(row);
		}
	}
}

int PatientTable::selectedPatientId() const {
	int row = currentRow();
	if (row < 0 || !item(row, 0)) {
		return -1;
	}
	return item(row, 0)->data(Qt::UserRole).toInt();
}

}
