#include "utils/IDGenerator.h"
#include "utils/Constants.h"

namespace scapms {

namespace {
int nextAppointment = 1;
}

int IDGenerator::nextPatientId(int highestExistingId) {
	int base = highestExistingId < constants::FIRST_PATIENT_ID ? constants::FIRST_PATIENT_ID : highestExistingId;
	return base + 1;
}

int IDGenerator::nextAppointmentId() {
	return nextAppointment++;
}

void IDGenerator::observeAppointmentId(int appointmentId) {
	if (appointmentId >= nextAppointment) {
		nextAppointment = appointmentId + 1;
	}
}

void IDGenerator::resetAppointmentIds() {
	nextAppointment = 1;
}

}
