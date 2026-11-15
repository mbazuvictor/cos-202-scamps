#pragma once

#include "controller/AppointmentController.h"
#include "controller/SearchEngine.h"
#include "model/ClinicDatabase.h"
#include "utils/DatabaseManager.h"
#include <ctime>
#include <optional>
#include <queue>
#include <stack>
#include <string>
#include <vector>

namespace scapms {

enum class UndoKind { DeletePatient, EditPatient, CancelAppointment };

struct UndoOperation {
	UndoKind kind;
	std::string description;
	int patientId = 0;
	int appointmentId = 0;
	std::string type;
	PatientDetails details;
	std::vector<Appointment> appointments;
};

struct QueueEntry {
	int patientId;
	std::time_t joined;
};

struct WaitInfo {
	int patientId;
	std::string patientName;
	long seconds;
};

class ClinicManager {
public:
	ClinicManager();
	int addPatient(const std::string &type, const PatientDetails &details, int requestedId = 0);
	void editPatient(int id, const PatientDetails &details);
	void deletePatient(int id);
	PatientRecord &patientById(int id);
	const PatientList &patients() const { return db.patients(); }
	int countCategory(const std::string &category) const;

	int scheduleAppointment(int patientId, const std::string &date);
	void cancelAppointment(int patientId, int appointmentId);
	void completeAppointment(int patientId, int appointmentId);
	const AppointmentController &appointmentController() const { return appointments; }

	std::string describeForConsultation(const PatientRecord &patient) const;
	void enqueue(int patientId);
	std::string callNext();
	PatientList queuedPatients() const;
	std::optional<WaitInfo> longestWait() const;

	SearchResult search(const std::string &mode, const std::string &query);
	void sort(const std::string &algorithm, const std::string &field);
	const std::string &sortedBy() const { return sortedField; }
	PatientList recentPatients() const;

	bool canUndo() const { return !undoStack.empty(); }
	int undoDepth() const { return static_cast<int>(undoStack.size()); }
	std::string nextUndoDescription() const;
	std::string undo();

	void save();
	void load();
	void newDatabase();
	void exportToFile(const std::string &path) const;
	void importFromFile(const std::string &path);

private:
	ClinicDatabase db;
	AppointmentController appointments;
	std::queue<QueueEntry> waiting;
	std::stack<UndoOperation> undoStack;
	PatientRecord *cache[10];
	int cacheNext = 0;
	std::string sortedField;
	DatabaseManager database;

	void touch(PatientRecord *patient);
	void forget(PatientRecord *patient);
	void resetSession();
	void removeFromQueue(int patientId);
	void applyRows(const std::vector<PatientRow> &patients, const std::vector<AppointmentRow> &appointmentRows);
	void snapshot(std::vector<PatientRow> &patients, std::vector<AppointmentRow> &appointmentRows) const;
	std::string revert(const UndoOperation &operation);
};

}
