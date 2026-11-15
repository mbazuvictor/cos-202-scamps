#pragma once

#include "gui/Panel.h"
#include "gui/PatientPicker.h"
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QTableWidget>

namespace scapms {

class AppointmentPanel : public Panel {
	Q_OBJECT

public:
	explicit AppointmentPanel(ClinicManager &manager, QWidget *parent = nullptr);
	void refresh() override;

private slots:
	void scheduleAppointment();
	void cancelAppointment();
	void completeAppointment();
	void warnIfPast();

private:
	template <typename Action>
	void actOnSelected(const QString &message, Action action);

	PatientPicker *picker;
	QDateEdit *dateEdit;
	QLabel *dateWarning;
	QComboBox *statusFilter;
	QTableWidget *table;
};

}
