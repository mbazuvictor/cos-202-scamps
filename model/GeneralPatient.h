#pragma once

#include "model/PatientRecord.h"

namespace scapms {

class GeneralPatient : public PatientRecord {
public:
	using PatientRecord::PatientRecord;
	std::string category() const override;
	std::string describe() const override;
	std::string priorityLabel() const override;
	std::string detailLabel() const override;
};

}
