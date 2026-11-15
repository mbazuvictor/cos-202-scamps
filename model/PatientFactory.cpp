#include "model/PatientFactory.h"
#include "model/EmergencyPatient.h"
#include "model/FollowUpPatient.h"
#include "model/GeneralPatient.h"
#include "utils/Constants.h"
#include "utils/Exceptions.h"

namespace scapms {

const std::vector<std::string> &PatientFactory::categories() {
	static const std::vector<std::string> names = {
		constants::CATEGORY_GENERAL,
		constants::CATEGORY_EMERGENCY,
		constants::CATEGORY_FOLLOW_UP,
	};
	return names;
}

void PatientFactory::validate(const std::string &type, const PatientDetails &details) {
	bool known = false;
	for (const auto &category : categories()) {
		known = known || category == type;
	}
	if (!known) {
		throw ClinicException("Error: Unknown patient type.");
	}
	PatientRecord::validate(details);
	bool badFollowUpDate = !details.detail.empty() && !Appointment::isValidDate(details.detail);
	if (type == constants::CATEGORY_FOLLOW_UP && badFollowUpDate) {
		throw ClinicException("Error: Previous visit date must use the format YYYY-MM-DD.");
	}
}

PatientRecord *PatientFactory::create(const std::string &type, int id, const PatientDetails &details) {
	validate(type, details);
	if (type == constants::CATEGORY_EMERGENCY) {
		return new EmergencyPatient(id, details);
	}
	if (type == constants::CATEGORY_FOLLOW_UP) {
		return new FollowUpPatient(id, details);
	}
	return new GeneralPatient(id, details);
}

}
