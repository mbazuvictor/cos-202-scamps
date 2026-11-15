#pragma once

#include "utils/Rows.h"
#include <string>
#include <vector>

namespace scapms {

class DatabaseManager {
public:
	explicit DatabaseManager(std::string file = "scapms.db");
	void initialise() const;
	void replace(const std::vector<PatientRow> &patients, const std::vector<AppointmentRow> &appointments) const;
	void read(std::vector<PatientRow> &patients, std::vector<AppointmentRow> &appointments) const;

private:
	std::string filename;
	void exec(const std::string &sql) const;
};

}
