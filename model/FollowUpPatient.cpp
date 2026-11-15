#include "model/FollowUpPatient.h"
#include "utils/Constants.h"
#include "utils/Exceptions.h"

namespace scapms {

std::string FollowUpPatient::category() const { return constants::CATEGORY_FOLLOW_UP; }
std::string FollowUpPatient::describe() const { return "Scheduled follow-up visit"; }
std::string FollowUpPatient::priorityLabel() const { return "Scheduled"; }
std::string FollowUpPatient::detailLabel() const { return "Previous visit date"; }

int FollowUpPatient::scheduleAppointment(const std::string &date) {
	if (Appointment::isValidDate(date) && Appointment::isValidDate(detail_) && date < detail_) {
		throw ClinicException("Error: A follow-up cannot be scheduled before the previous visit (" + detail_ + ").");
	}
	return PatientRecord::scheduleAppointment(date);
}

}
