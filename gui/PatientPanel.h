#pragma once

#include "gui/Panel.h"
#include "gui/PatientTable.h"

namespace scapms {

class PatientPanel : public Panel {
	Q_OBJECT

public:
	explicit PatientPanel(ClinicManager &manager, QWidget *parent = nullptr);
	void refresh() override;

public slots:
	void addPatient();
	void editPatient();
	void deletePatient();

private:
	PatientTable *table;
};

}
