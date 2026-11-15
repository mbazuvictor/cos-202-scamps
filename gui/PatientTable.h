#pragma once

#include "model/PatientRecord.h"
#include <QTableWidget>

namespace scapms {

class PatientTable : public QTableWidget {
	Q_OBJECT

public:
	explicit PatientTable(QWidget *parent = nullptr);
	void setPatients(const PatientList &patients);
	int selectedPatientId() const;
};

}
