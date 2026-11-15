#pragma once

#include "model/ClinicDatabase.h"
#include <string>
#include <vector>

namespace scapms {

struct AppointmentEntry {
	PatientRecord *patient;
	Appointment *appointment;
};

struct AppointmentSummary {
	int total = 0;
	int completed = 0;
	int pending = 0;
	int cancelled = 0;
};

class AppointmentController {
public:
	explicit AppointmentController(ClinicDatabase &database) : database(database) {}
	int schedule(int patientId, const std::string &date);
	void cancel(int patientId, int appointmentId);
	void complete(int patientId, int appointmentId);
	void reopen(int patientId, int appointmentId);
	std::vector<AppointmentEntry> entries() const;
	std::vector<AppointmentEntry> pendingOn(const std::string &date) const;
	AppointmentSummary summary() const;
	PatientList mostAttended() const;
	PatientList withPending() const;

private:
	ClinicDatabase &database;
};

}
