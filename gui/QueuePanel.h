#pragma once

#include "gui/Panel.h"
#include "gui/PatientPicker.h"
#include "gui/PatientTable.h"
#include <QLabel>
#include <QSpinBox>
#include <QTimer>

namespace scapms {

class QueuePanel : public Panel {
	Q_OBJECT

public:
	explicit QueuePanel(ClinicManager &manager, QWidget *parent = nullptr);
	void refresh() override;

private slots:
	void enqueuePatient();
	void callNext();
	void updateWaitStatus();

private:
	PatientPicker *picker;
	PatientTable *table;
	QSpinBox *alertMinutes;
	QLabel *waitLabel;
	QTimer *timer;
	int alertedPatientId = -1;
};

}
