#pragma once

#include "model/PatientRecord.h"
#include <QComboBox>

namespace scapms {

class PatientPicker : public QComboBox {
	Q_OBJECT

public:
	explicit PatientPicker(QWidget *parent = nullptr);
	void setPatients(const PatientList &patients);
	int selectedPatientId() const;
};

}
