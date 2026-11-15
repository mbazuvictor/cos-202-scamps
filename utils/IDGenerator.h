#pragma once

namespace scapms {

class IDGenerator {
public:
	static int nextPatientId(int highestExistingId);
	static int nextAppointmentId();
	static void observeAppointmentId(int appointmentId);
	static void resetAppointmentIds();
};

}
