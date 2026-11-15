#include "controller/AppointmentController.h"
#include "utils/Constants.h"
#include "utils/Exceptions.h"

namespace scapms {

int AppointmentController::schedule(int patientId, const std::string &date) {
	return database.require(patientId).scheduleAppointment(date);
}

void AppointmentController::cancel(int patientId, int appointmentId) {
	if (!database.require(patientId).cancelAppointment(appointmentId)) {
		throw ClinicException("Error: No pending appointment with that ID.");
	}
}

void AppointmentController::complete(int patientId, int appointmentId) {
	if (!database.require(patientId).completeAppointment(appointmentId)) {
		throw ClinicException("Error: No pending appointment with that ID.");
	}
}

void AppointmentController::reopen(int patientId, int appointmentId) {
	if (!database.require(patientId).reopenAppointment(appointmentId)) {
		throw ClinicException("Error: Appointment not found.");
	}
}

std::vector<AppointmentEntry> AppointmentController::entries() const {
	std::vector<AppointmentEntry> all;
	for (auto *patient : database.patients()) {
		for (auto &appointment : patient->appointments()) {
			all.push_back({patient, &appointment});
		}
	}
	return all;
}

std::vector<AppointmentEntry> AppointmentController::pendingOn(const std::string &date) const {
	std::vector<AppointmentEntry> matches;
	for (const auto &entry : entries()) {
		if (entry.appointment->isPending() && entry.appointment->date() == date) {
			matches.push_back(entry);
		}
	}
	return matches;
}

AppointmentSummary AppointmentController::summary() const {
	AppointmentSummary result;
	for (const auto &entry : entries()) {
		result.total++;
		result.completed += entry.appointment->isCompleted() ? 1 : 0;
		result.pending += entry.appointment->isPending() ? 1 : 0;
		result.cancelled += entry.appointment->isCancelled() ? 1 : 0;
	}
	return result;
}

PatientList AppointmentController::mostAttended() const {
	PatientList ranked;
	for (auto *patient : database.patients()) {
		if (patient->countStatus(constants::STATUS_COMPLETED) > 0) {
			ranked.push_back(patient);
		}
	}
	auto completedCount = [](const PatientRecord *patient) { return patient->countStatus(constants::STATUS_COMPLETED); };
	for (size_t i = 0; i + 1 < ranked.size(); i++) {
		size_t best = i;
		for (size_t j = i + 1; j < ranked.size(); j++) {
			if (completedCount(ranked[j]) > completedCount(ranked[best])) {
				best = j;
			}
		}
		PatientRecord *held = ranked[i];
		ranked[i] = ranked[best];
		ranked[best] = held;
	}
	return ranked;
}

PatientList AppointmentController::withPending() const {
	PatientList pending;
	for (auto *patient : database.patients()) {
		if (patient->countStatus(constants::STATUS_PENDING) > 0) {
			pending.push_back(patient);
		}
	}
	return pending;
}

}
