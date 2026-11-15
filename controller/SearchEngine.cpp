#include "controller/SearchEngine.h"
#include "utils/Algorithms.h"
#include "utils/Constants.h"
#include "utils/Exceptions.h"

namespace scapms {

namespace {

std::string trim(const std::string &text) {
	size_t first = text.find_first_not_of(" \t");
	if (first == std::string::npos) {
		return "";
	}
	return text.substr(first, text.find_last_not_of(" \t") - first + 1);
}

std::string canonicalCategory(const std::string &text) {
	std::string key;
	for (char character : algo::lower(text)) {
		if (character != ' ' && character != '-') {
			key += character;
		}
	}
	if (key == "general") {
		return constants::CATEGORY_GENERAL;
	}
	if (key == "emergency") {
		return constants::CATEGORY_EMERGENCY;
	}
	if (key == "followup") {
		return constants::CATEGORY_FOLLOW_UP;
	}
	throw ClinicException("Error: Category must be General, Emergency or FollowUp.");
}

int parseId(const std::string &text) {
	if (text.empty() || text.size() > 9 || text.find_first_not_of("0123456789") != std::string::npos) {
		throw ClinicException("Error: Patient ID must be a number.");
	}
	return std::stoi(text);
}

SearchResult searchById(const PatientList &patients, const std::string &query, const std::string &sortedBy) {
	int id = parseId(query);
	bool sorted = sortedBy == constants::SORT_ID;
	int index = sorted ? algo::binarySearchById(patients, id) : algo::linearSearchById(patients, id);
	SearchResult result;
	result.algorithm = sorted ? "Binary Search (list sorted by ID)" : "Linear Search (list unsorted)";
	if (index >= 0) {
		result.patients.push_back(patients[index]);
	}
	return result;
}

SearchResult searchByName(const PatientList &patients, const std::string &query, const std::string &sortedBy) {
	SearchResult result;
	if (sortedBy == constants::SORT_NAME) {
		int first = algo::binarySearchFirstByName(patients, query);
		std::string target = algo::lower(query);
		for (size_t i = first < 0 ? patients.size() : first; i < patients.size(); i++) {
			if (algo::lower(patients[i]->name()) != target) {
				break;
			}
			result.patients.push_back(patients[i]);
		}
		if (!result.patients.empty()) {
			result.algorithm = "Binary Search (exact name, list sorted by name)";
			return result;
		}
	}
	algo::recursiveSearchName(patients, 0, query, result.patients);
	result.algorithm = "Recursive Search (name contains keyword)";
	return result;
}

SearchResult searchByCategory(const PatientList &patients, const std::string &query) {
	std::string category = canonicalCategory(query);
	SearchResult result;
	result.algorithm = "Linear Search (category)";
	for (auto *patient : patients) {
		if (patient->category() == category) {
			result.patients.push_back(patient);
		}
	}
	return result;
}

}

SearchResult SearchEngine::search(const PatientList &patients, const std::string &mode, const std::string &query,
	const std::string &sortedBy) {
	std::string cleaned = trim(query);
	if (cleaned.empty()) {
		throw ClinicException("Error: Enter something to search for.");
	}
	if (mode == constants::SEARCH_ID) {
		return searchById(patients, cleaned, sortedBy);
	}
	if (mode == constants::SEARCH_NAME) {
		return searchByName(patients, cleaned, sortedBy);
	}
	if (mode == constants::SEARCH_CATEGORY) {
		return searchByCategory(patients, cleaned);
	}
	throw ClinicException("Error: Unknown search type.");
}

}
