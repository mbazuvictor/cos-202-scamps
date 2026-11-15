#pragma once

#include "model/Appointment.h"
#include "model/PatientDetails.h"
#include <string>
#include <vector>

namespace scapms {

class Appointable {
public:
	virtual ~Appointable() = default;
	virtual int scheduleAppointment(const std::string &date) = 0;
	virtual bool cancelAppointment(int appointmentId) = 0;
};

class PatientRecord : public Appointable {
public:
	PatientRecord(int id, PatientDetails details);
	virtual std::string category() const = 0;
	virtual std::string describe() const = 0;
	virtual std::string priorityLabel() const = 0;
	virtual std::string detailLabel() const = 0;
	static void validate(const PatientDetails &details);
	static void validateId(int id);
	void setDetails(const PatientDetails &details);
	int id() const { return id_; }
	int age() const { return age_; }
	const std::string &name() const { return name_; }
	const std::string &contact() const { return contact_; }
	const std::string &detail() const { return detail_; }
	std::vector<Appointment> &appointments() { return appointments_; }
	const std::vector<Appointment> &appointments() const { return appointments_; }
	int scheduleAppointment(const std::string &date) override;
	bool cancelAppointment(int appointmentId) override;
	bool completeAppointment(int appointmentId);
	bool reopenAppointment(int appointmentId);
	int countStatus(const std::string &status) const;
	std::string nextAppointmentDate() const;

protected:
	int id_;
	int age_;
	std::string name_;
	std::string contact_;
	std::string detail_;
	std::vector<Appointment> appointments_;
};

typedef std::vector<PatientRecord *> PatientList;

}
