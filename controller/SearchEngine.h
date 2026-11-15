#pragma once

#include "model/PatientRecord.h"
#include <string>

namespace scapms {

struct SearchResult {
	PatientList patients;
	std::string algorithm;
};

class SearchEngine {
public:
	static SearchResult search(const PatientList &patients, const std::string &mode, const std::string &query,
		const std::string &sortedBy);
};

}
