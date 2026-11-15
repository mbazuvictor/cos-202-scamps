#include "controller/ClinicManager.h"
#include "model/PatientFactory.h"
#include "utils/Algorithms.h"
#include "utils/Constants.h"
#include "utils/Exceptions.h"
#include "utils/FileHandler.h"
#include "utils/IDGenerator.h"

namespace scapms {

ClinicManager::ClinicManager() : appointments(db), database(constants::DATABASE_FILE) {
	resetSession();
}

void ClinicManager::touch(PatientRecord *patient) {
	for (auto *cached : cache) {
		if (cached == patient) {
			return;
		}
	}
	cache[cacheNext] = patient;
	cacheNext = (cacheNext + 1) % constants::CACHE_SIZE;
}

void ClinicManager::forget(PatientRecord *patient) {
	for (auto &cached : cache) {
		if (cached == patient) {
			cached = nullptr;
		}
	}
}

void ClinicManager::resetSession() {
	for (auto &cached : cache) {
		cached = nullptr;
	}
	cacheNext = 0;
	waiting = std::queue<QueueEntry>();
	undoStack = std::stack<UndoOperation>();
	sortedField.clear();
}

void ClinicManager::removeFromQueue(int patientId) {
	std::queue<QueueEntry> remaining;
	while (!waiting.empty()) {
		if (waiting.front().patientId != patientId) {
			remaining.push(waiting.front());
		}
		waiting.pop();
	}
	waiting = remaining;
}

int ClinicManager::addPatient(const std::string &type, const PatientDetails &details, int requestedId) {
	int id = requestedId;
	if (requestedId == 0) {
		id = IDGenerator::nextPatientId(db.highestId());
	}
	PatientRecord::validateId(id);
	if (db.contains(id)) {
		throw ClinicException("Error: Patient ID already exists.");
	}
	db.add(PatientFactory::create(type, id, details));
	sortedField.clear();
	return id;
}

void ClinicManager::editPatient(int id, const PatientDetails &details) {
	PatientRecord &patient = db.require(id);
	PatientFactory::validate(patient.category(), details);
	UndoOperation operation;
	operation.kind = UndoKind::EditPatient;
	operation.description = "Edit of " + patient.name();
	operation.patientId = id;
	operation.details = {patient.name(), patient.age(), patient.contact(), patient.detail()};
	patient.setDetails(details);
	undoStack.push(operation);
	touch(&patient);
}

void ClinicManager::deletePatient(int id) {
	PatientRecord &patient = db.require(id);
	UndoOperation operation;
	operation.kind = UndoKind::DeletePatient;
	operation.description = "Deletion of " + patient.name();
	operation.patientId = id;
	operation.type = patient.category();
	operation.details = {patient.name(), patient.age(), patient.contact(), patient.detail()};
	operation.appointments = patient.appointments();
	undoStack.push(operation);
	forget(&patient);
	removeFromQueue(id);
	db.erase(id);
}

PatientRecord &ClinicManager::patientById(int id) {
	return db.require(id);
}

int ClinicManager::countCategory(const std::string &category) const {
	int count = 0;
	for (const auto *patient : db.patients()) {
		count += patient->category() == category ? 1 : 0;
	}
	return count;
}

int ClinicManager::scheduleAppointment(int patientId, const std::string &date) {
	int appointmentId = appointments.schedule(patientId, date);
	touch(&db.require(patientId));
	return appointmentId;
}

void ClinicManager::cancelAppointment(int patientId, int appointmentId) {
	appointments.cancel(patientId, appointmentId);
	UndoOperation operation;
	operation.kind = UndoKind::CancelAppointment;
	operation.description = "Cancellation of appointment #" + std::to_string(appointmentId);
	operation.patientId = patientId;
	operation.appointmentId = appointmentId;
	undoStack.push(operation);
}

void ClinicManager::completeAppointment(int patientId, int appointmentId) {
	appointments.complete(patientId, appointmentId);
}

std::string ClinicManager::describeForConsultation(const PatientRecord &patient) const {
	return patient.describe();
}

void ClinicManager::enqueue(int patientId) {
	db.require(patientId);
	std::queue<QueueEntry> copy = waiting;
	while (!copy.empty()) {
		if (copy.front().patientId == patientId) {
			throw ClinicException("Error: Patient is already in the queue.");
		}
		copy.pop();
	}
	waiting.push({patientId, std::time(nullptr)});
}

std::string ClinicManager::callNext() {
	if (waiting.empty()) {
		throw ClinicException("The waiting queue is empty.");
	}
	int patientId = waiting.front().patientId;
	waiting.pop();
	PatientRecord &patient = db.require(patientId);
	touch(&patient);
	return "Now calling " + patient.name() + ": " + describeForConsultation(patient);
}

PatientList ClinicManager::queuedPatients() const {
	PatientList list;
	std::queue<QueueEntry> copy = waiting;
	while (!copy.empty()) {
		PatientRecord *patient = db.find(copy.front().patientId);
		if (patient) {
			list.push_back(patient);
		}
		copy.pop();
	}
	return list;
}

std::optional<WaitInfo> ClinicManager::longestWait() const {
	if (waiting.empty()) {
		return std::nullopt;
	}
	const QueueEntry &front = waiting.front();
	PatientRecord *patient = db.find(front.patientId);
	if (!patient) {
		return std::nullopt;
	}
	return WaitInfo{front.patientId, patient->name(), static_cast<long>(std::time(nullptr) - front.joined)};
}

SearchResult ClinicManager::search(const std::string &mode, const std::string &query) {
	SearchResult result = SearchEngine::search(db.patients(), mode, query, sortedField);
	for (auto *patient : result.patients) {
		touch(patient);
	}
	return result;
}

void ClinicManager::sort(const std::string &algorithm, const std::string &field) {
	bool knownField = field == constants::SORT_ID || field == constants::SORT_NAME ||
		field == constants::SORT_AGE || field == constants::SORT_DATE;
	if (!knownField) {
		throw ClinicException("Error: Invalid sort field.");
	}
	if (algorithm == constants::ALGORITHM_SELECTION) {
		algo::selectionSort(db.patients(), field);
	} else if (algorithm == constants::ALGORITHM_MERGE) {
		algo::mergeSort(db.patients(), 0, static_cast<int>(db.patients().size()) - 1, field);
	} else {
		throw ClinicException("Error: Unknown sorting algorithm.");
	}
	sortedField = field;
}

PatientList ClinicManager::recentPatients() const {
	PatientList recent;
	for (auto *cached : cache) {
		if (cached) {
			recent.push_back(cached);
		}
	}
	return recent;
}

std::string ClinicManager::nextUndoDescription() const {
	return undoStack.empty() ? "" : undoStack.top().description;
}

std::string ClinicManager::undo() {
	if (undoStack.empty()) {
		throw ClinicException("Nothing to undo.");
	}
	UndoOperation operation = undoStack.top();
	undoStack.pop();
	return revert(operation);
}

std::string ClinicManager::revert(const UndoOperation &operation) {
	if (operation.kind == UndoKind::DeletePatient) {
		if (db.contains(operation.patientId)) {
			throw ClinicException("Error: Patient ID already exists, so the deleted record cannot be restored.");
		}
		PatientRecord *restored = PatientFactory::create(operation.type, operation.patientId, operation.details);
		restored->appointments() = operation.appointments;
		db.add(restored);
		sortedField.clear();
		return "Restored patient " + operation.details.name;
	}
	if (operation.kind == UndoKind::EditPatient) {
		db.require(operation.patientId).setDetails(operation.details);
		return "Reverted edit of " + operation.details.name;
	}
	appointments.reopen(operation.patientId, operation.appointmentId);
	return "Restored cancelled appointment #" + std::to_string(operation.appointmentId);
}

void ClinicManager::snapshot(std::vector<PatientRow> &patients, std::vector<AppointmentRow> &appointmentRows) const {
	for (const auto *patient : db.patients()) {
		patients.push_back({patient->id(), patient->category(), patient->name(), patient->age(),
			patient->contact(), patient->detail()});
		for (const auto &appointment : patient->appointments()) {
			appointmentRows.push_back({appointment.id(), appointment.patientId(),
				appointment.date(), appointment.status()});
		}
	}
}

void ClinicManager::applyRows(const std::vector<PatientRow> &patients,
	const std::vector<AppointmentRow> &appointmentRows) {
	ClinicDatabase fresh;
	for (const auto &row : patients) {
		PatientRecord::validateId(row.id);
		if (fresh.contains(row.id)) {
			throw ClinicException("Error: Patient ID already exists (" + std::to_string(row.id) + ").");
		}
		fresh.add(PatientFactory::create(row.type, row.id, {row.name, row.age, row.contact, row.detail}));
	}
	for (const auto &row : appointmentRows) {
		PatientRecord *patient = fresh.find(row.patientId);
		if (!patient) {
			throw ClinicException("Error: An appointment refers to an unknown patient.");
		}
		if (!Appointment::isValidDate(row.date) || !Appointment::isValidStatus(row.status)) {
			throw ClinicException("Error: Invalid appointment data for appointment " + std::to_string(row.id) + ".");
		}
		patient->appointments().emplace_back(row.id, row.patientId, row.date, row.status);
	}
	resetSession();
	db.swap(fresh);
	IDGenerator::resetAppointmentIds();
	for (const auto &row : appointmentRows) {
		IDGenerator::observeAppointmentId(row.id);
	}
}

void ClinicManager::save() {
	std::vector<PatientRow> patients;
	std::vector<AppointmentRow> appointmentRows;
	snapshot(patients, appointmentRows);
	database.replace(patients, appointmentRows);
}

void ClinicManager::load() {
	std::vector<PatientRow> patients;
	std::vector<AppointmentRow> appointmentRows;
	database.read(patients, appointmentRows);
	applyRows(patients, appointmentRows);
}

void ClinicManager::newDatabase() {
	applyRows({}, {});
}

void ClinicManager::exportToFile(const std::string &path) const {
	std::vector<PatientRow> patients;
	std::vector<AppointmentRow> appointmentRows;
	snapshot(patients, appointmentRows);
	FileHandler::writeCsv(path, patients, appointmentRows);
}

void ClinicManager::importFromFile(const std::string &path) {
	std::vector<PatientRow> patients;
	std::vector<AppointmentRow> appointmentRows;
	FileHandler::readCsv(path, patients, appointmentRows);
	applyRows(patients, appointmentRows);
}

}
