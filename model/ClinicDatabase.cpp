#include "model/ClinicDatabase.h"
#include "utils/Algorithms.h"
#include "utils/Exceptions.h"

namespace scapms {

ClinicDatabase::~ClinicDatabase() {
	clear();
}

PatientRecord *ClinicDatabase::find(int id) const {
	int index = algo::linearSearchById(patients_, id);
	return index < 0 ? nullptr : patients_[index];
}

PatientRecord &ClinicDatabase::require(int id) const {
	PatientRecord *patient = find(id);
	if (!patient) {
		throw ClinicException("Error: Patient ID not found.");
	}
	return *patient;
}

bool ClinicDatabase::contains(int id) const {
	return find(id) != nullptr;
}

int ClinicDatabase::highestId() const {
	int highest = 0;
	for (const auto *patient : patients_) {
		if (patient->id() > highest) {
			highest = patient->id();
		}
	}
	return highest;
}

void ClinicDatabase::add(PatientRecord *patient) {
	patients_.push_back(patient);
}

void ClinicDatabase::erase(int id) {
	int index = algo::linearSearchById(patients_, id);
	if (index < 0) {
		return;
	}
	delete patients_[index];
	patients_.erase(patients_.begin() + index);
}

void ClinicDatabase::clear() {
	for (auto *patient : patients_) {
		delete patient;
	}
	patients_.clear();
}

void ClinicDatabase::swap(ClinicDatabase &other) {
	patients_.swap(other.patients_);
}

}
