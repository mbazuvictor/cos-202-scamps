#pragma once

#include "model/PatientRecord.h"

namespace scapms {

class ClinicDatabase {
public:
	ClinicDatabase() = default;
	ClinicDatabase(const ClinicDatabase &) = delete;
	ClinicDatabase &operator=(const ClinicDatabase &) = delete;
	~ClinicDatabase();
	PatientList &patients() { return patients_; }
	const PatientList &patients() const { return patients_; }
	PatientRecord *find(int id) const;
	PatientRecord &require(int id) const;
	bool contains(int id) const;
	int highestId() const;
	void add(PatientRecord *patient);
	void erase(int id);
	void clear();
	void swap(ClinicDatabase &other);

private:
	PatientList patients_;
};

}
