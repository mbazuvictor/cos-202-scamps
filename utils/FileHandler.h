#pragma once

#include "utils/Rows.h"
#include <string>
#include <vector>

namespace scapms {

class FileHandler {
public:
	static void writeCsv(const std::string &path, const std::vector<PatientRow> &patients,
		const std::vector<AppointmentRow> &appointments);
	static void readCsv(const std::string &path, std::vector<PatientRow> &patients,
		std::vector<AppointmentRow> &appointments);
};

}
