#pragma once

#include <string>

namespace scapms {

struct PatientRow {
	int id;
	std::string type;
	std::string name;
	int age;
	std::string contact;
	std::string detail;
};

struct AppointmentRow {
	int id;
	int patientId;
	std::string date;
	std::string status;
};

}
