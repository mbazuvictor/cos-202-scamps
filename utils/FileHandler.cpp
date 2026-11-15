#include "utils/FileHandler.h"
#include "utils/Constants.h"
#include "utils/Exceptions.h"
#include <fstream>
#include <sstream>

namespace scapms {

namespace {

std::vector<std::string> splitFields(const std::string &line) {
	std::vector<std::string> fields;
	std::string field;
	std::istringstream stream(line);
	while (std::getline(stream, field, ',')) {
		fields.push_back(field);
	}
	if (!line.empty() && line.back() == ',') {
		fields.push_back("");
	}
	return fields;
}

int parseNumber(const std::string &text, int lineNumber) {
	try {
		size_t used = 0;
		int value = std::stoi(text, &used);
		if (used == text.size()) {
			return value;
		}
	} catch (const std::exception &) {
	}
	throw ClinicException("Error: Invalid number on line " + std::to_string(lineNumber) + " of the file.");
}

void requireFieldCount(const std::vector<std::string> &fields, size_t expected, int lineNumber) {
	if (fields.size() != expected) {
		throw ClinicException("Error: Line " + std::to_string(lineNumber) + " of the file is malformed.");
	}
}

}

void FileHandler::writeCsv(const std::string &path, const std::vector<PatientRow> &patients,
	const std::vector<AppointmentRow> &appointments) {
	std::ofstream out(path);
	if (!out) {
		throw ClinicException("Error: Could not open the file for writing: " + path);
	}
	out << constants::CSV_SIGNATURE << "," << constants::CSV_VERSION << "\n";
	for (const auto &p : patients) {
		out << "P," << p.id << "," << p.type << "," << p.name << "," << p.age << ","
			<< p.contact << "," << p.detail << "\n";
	}
	for (const auto &a : appointments) {
		out << "A," << a.id << "," << a.patientId << "," << a.date << "," << a.status << "\n";
	}
	if (!out) {
		throw ClinicException("Error: Writing to the file failed: " + path);
	}
}

void FileHandler::readCsv(const std::string &path, std::vector<PatientRow> &patients,
	std::vector<AppointmentRow> &appointments) {
	std::ifstream in(path);
	if (!in) {
		throw ClinicException("Error: Could not open the file for reading: " + path);
	}
	std::string line;
	std::getline(in, line);
	if (line.rfind(std::string(constants::CSV_SIGNATURE) + ",", 0) != 0) {
		throw ClinicException("Error: This is not a SCAPMS data file.");
	}
	int lineNumber = 1;
	while (std::getline(in, line)) {
		lineNumber++;
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		if (line.empty()) {
			continue;
		}
		std::vector<std::string> fields = splitFields(line);
		if (fields[0] == "P") {
			requireFieldCount(fields, 7, lineNumber);
			patients.push_back({parseNumber(fields[1], lineNumber), fields[2], fields[3],
				parseNumber(fields[4], lineNumber), fields[5], fields[6]});
		} else if (fields[0] == "A") {
			requireFieldCount(fields, 5, lineNumber);
			appointments.push_back({parseNumber(fields[1], lineNumber), parseNumber(fields[2], lineNumber),
				fields[3], fields[4]});
		} else {
			throw ClinicException("Error: Unknown record type on line " + std::to_string(lineNumber) + " of the file.");
		}
	}
}

}
