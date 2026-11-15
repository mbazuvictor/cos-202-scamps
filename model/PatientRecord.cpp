#include "model/PatientRecord.h"
#include "utils/Constants.h"
#include "utils/Exceptions.h"
#include "utils/IDGenerator.h"
#include <utility>

namespace scapms {

namespace {

bool containsForbidden(const std::string &text) {
	return text.find_first_of(constants::FORBIDDEN_CHARACTERS) != std::string::npos;
}

}

PatientRecord::PatientRecord(int id, PatientDetails details)
	: id_(id), age_(details.age), name_(std::move(details.name)), contact_(std::move(details.contact)),
	  detail_(std::move(details.detail)) {}

void PatientRecord::validate(const PatientDetails &details) {
	if (details.name.empty()) {
		throw ClinicException("Error: Patient name is required.");
	}
	if (details.age < constants::MIN_AGE || details.age > constants::MAX_AGE) {
		throw ClinicException("Error: Patient age must be between 0 and 120.");
	}
	if (details.contact.empty()) {
		throw ClinicException("Error: Contact information is required.");
	}
	bool hasForbidden = containsForbidden(details.name) || containsForbidden(details.contact) ||
		containsForbidden(details.detail);
	if (hasForbidden) {
		throw ClinicException("Error: Commas, quotes, backslashes and line breaks are not allowed.");
	}
}

void PatientRecord::validateId(int id) {
	if (id < 1 || id > constants::MAX_PATIENT_ID) {
		throw ClinicException("Error: Patient ID must be a number between 1 and 999999.");
	}
}

void PatientRecord::setDetails(const PatientDetails &details) {
	validate(details);
	name_ = details.name;
	age_ = details.age;
	contact_ = details.contact;
	detail_ = details.detail;
}

int PatientRecord::scheduleAppointment(const std::string &date) {
	if (!Appointment::isValidDate(date)) {
		throw ClinicException("Error: Invalid appointment date (use YYYY-MM-DD).");
	}
	appointments_.emplace_back(IDGenerator::nextAppointmentId(), id_, date, constants::STATUS_PENDING);
	return appointments_.back().id();
}

bool PatientRecord::cancelAppointment(int appointmentId) {
	for (auto &appointment : appointments_) {
		if (appointment.id() == appointmentId && appointment.isPending()) {
			appointment.markCancelled();
			return true;
		}
	}
	return false;
}

bool PatientRecord::completeAppointment(int appointmentId) {
	for (auto &appointment : appointments_) {
		if (appointment.id() == appointmentId && appointment.isPending()) {
			appointment.markCompleted();
			return true;
		}
	}
	return false;
}

bool PatientRecord::reopenAppointment(int appointmentId) {
	for (auto &appointment : appointments_) {
		if (appointment.id() == appointmentId) {
			appointment.markPending();
			return true;
		}
	}
	return false;
}

int PatientRecord::countStatus(const std::string &status) const {
	int count = 0;
	for (const auto &appointment : appointments_) {
		if (appointment.status() == status) {
			count++;
		}
	}
	return count;
}

std::string PatientRecord::nextAppointmentDate() const {
	std::string earliest;
	for (const auto &appointment : appointments_) {
		if (appointment.isPending() && (earliest.empty() || appointment.date() < earliest)) {
			earliest = appointment.date();
		}
	}
	return earliest;
}

}
