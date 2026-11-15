#pragma once

#include "model/PatientRecord.h"

namespace scapms {

class FollowUpPatient : public PatientRecord {
public:
	using PatientRecord::PatientRecord;
	std::string category() const override;
	std::string describe() const override;
	std::string priorityLabel() const override;
	std::string detailLabel() const override;
	int scheduleAppointment(const std::string &date) override;
};

}
