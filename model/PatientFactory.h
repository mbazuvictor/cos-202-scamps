#pragma once

#include "model/PatientRecord.h"
#include <string>
#include <vector>

namespace scapms {

class PatientFactory {
public:
	static const std::vector<std::string> &categories();
	static void validate(const std::string &type, const PatientDetails &details);
	static PatientRecord *create(const std::string &type, int id, const PatientDetails &details);
};

}
