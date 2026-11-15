#pragma once

#include <string>

namespace scapms {

struct PatientDetails {
	std::string name;
	int age = 0;
	std::string contact;
	std::string detail;
};

}
