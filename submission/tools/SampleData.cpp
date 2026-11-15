#include "SampleData.h"
#include "controller/ClinicManager.h"
#include <QDate>
#include <QDir>
#include <QFile>

namespace {

std::string daysFromToday(int days) {
	return QDate::currentDate().addDays(days).toString("yyyy-MM-dd").toStdString();
}

int addPatient(scapms::ClinicManager &manager, const char *type, const char *name, int age, const char *contact,
	const std::string &detail) {
	return manager.addPatient(type, {name, age, contact, detail});
}

void book(scapms::ClinicManager &manager, int patientId, int days, bool complete) {
	int appointmentId = manager.scheduleAppointment(patientId, daysFromToday(days));
	if (complete) {
		manager.completeAppointment(patientId, appointmentId);
	}
}

}

void seedSampleData(const QString &workDirectory, const QString &csvPath) {
	QDir().mkpath(workDirectory);
	QDir::setCurrent(workDirectory);
	QFile::remove("scapms.db");
	scapms::ClinicManager manager;
	manager.newDatabase();
	int chidi = addPatient(manager, "General", "Chidi Okeke", 34, "0803-555-0101", "Persistent cough");
	int amina = addPatient(manager, "General", "Amina Bello", 28, "0805-555-0102", "Headache");
	int tunde = addPatient(manager, "General", "Tunde Adeyemi", 52, "0806-555-0103", "Blood pressure check");
	int ngozi = addPatient(manager, "General", "Ngozi Eze", 41, "0807-555-0104", "Back pain");
	int ibrahim = addPatient(manager, "General", "Ibrahim Musa", 23, "0808-555-0105", "Malaria symptoms");
	int grace = addPatient(manager, "General", "Grace Okafor", 67, "0809-555-0106", "Routine checkup");
	int emeka = addPatient(manager, "Emergency", "Emeka Nwosu", 45, "0810-555-0107", "Chest pain");
	int fatima = addPatient(manager, "Emergency", "Fatima Yusuf", 8, "0811-555-0108", "High fever - mother Hauwa");
	addPatient(manager, "Emergency", "Samuel Adebayo", 31, "0812-555-0109", "Road accident injury");
	int blessing = addPatient(manager, "FollowUp", "Blessing Udo", 36, "0813-555-0110", daysFromToday(-14));
	int kelechi = addPatient(manager, "FollowUp", "Kelechi Obi", 59, "0814-555-0111", daysFromToday(-30));
	int halima = addPatient(manager, "FollowUp", "Halima Sani", 19, "0815-555-0112", daysFromToday(-7));

	book(manager, chidi, -20, true);
	book(manager, chidi, -12, true);
	book(manager, chidi, -3, true);
	book(manager, chidi, 1, false);
	book(manager, amina, -9, true);
	book(manager, amina, -4, true);
	book(manager, tunde, -6, true);
	book(manager, grace, -2, true);
	book(manager, blessing, -10, true);
	book(manager, blessing, -5, true);
	book(manager, ngozi, 0, false);
	book(manager, ibrahim, 3, false);
	book(manager, kelechi, 1, false);
	book(manager, halima, 5, false);
	book(manager, emeka, 0, false);
	manager.cancelAppointment(amina, manager.scheduleAppointment(amina, daysFromToday(2)));
	manager.cancelAppointment(fatima, manager.scheduleAppointment(fatima, daysFromToday(4)));
	manager.save();
	manager.exportToFile(csvPath.toStdString());
}
