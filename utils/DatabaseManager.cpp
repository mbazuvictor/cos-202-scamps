#include "utils/DatabaseManager.h"
#include "utils/Exceptions.h"
#include <sqlite3.h>
#include <utility>

namespace scapms {

namespace {

const char *CREATE_TABLES_SQL =
	"PRAGMA foreign_keys = ON;"
	"CREATE TABLE IF NOT EXISTS patients ("
	"id INTEGER PRIMARY KEY, type TEXT NOT NULL, name TEXT NOT NULL, "
	"age INTEGER NOT NULL, contact TEXT NOT NULL, detail TEXT NOT NULL DEFAULT '');"
	"CREATE TABLE IF NOT EXISTS appointments ("
	"id INTEGER PRIMARY KEY, patient_id INTEGER NOT NULL, appointment_date TEXT NOT NULL, status TEXT NOT NULL, "
	"FOREIGN KEY(patient_id) REFERENCES patients(id) ON DELETE CASCADE);";

const char *ADD_DETAIL_COLUMN_SQL = "ALTER TABLE patients ADD COLUMN detail TEXT NOT NULL DEFAULT ''";

const char *INSERT_PATIENT_SQL =
	"INSERT INTO patients(id, type, name, age, contact, detail) VALUES(?, ?, ?, ?, ?, ?)";
const char *INSERT_APPOINTMENT_SQL =
	"INSERT INTO appointments(id, patient_id, appointment_date, status) VALUES(?, ?, ?, ?)";
const char *SELECT_PATIENTS_SQL = "SELECT id, type, name, age, contact, detail FROM patients ORDER BY id";
const char *SELECT_APPOINTMENTS_SQL = "SELECT id, patient_id, appointment_date, status FROM appointments ORDER BY id";

class Connection {
public:
	explicit Connection(const std::string &file) {
		if (sqlite3_open(file.c_str(), &handle) != SQLITE_OK) {
			std::string message = sqlite3_errmsg(handle);
			sqlite3_close(handle);
			throw ClinicException("Database error while opening database: " + message);
		}
	}

	Connection(const Connection &) = delete;
	Connection &operator=(const Connection &) = delete;

	~Connection() {
		sqlite3_close(handle);
	}

	sqlite3 *get() const {
		return handle;
	}

	void exec(const std::string &sql) const {
		char *error = nullptr;
		if (sqlite3_exec(handle, sql.c_str(), nullptr, nullptr, &error) != SQLITE_OK) {
			std::string message = error ? error : "unknown error";
			sqlite3_free(error);
			throw ClinicException("Database error: " + message);
		}
	}

private:
	sqlite3 *handle = nullptr;
};

class Statement {
public:
	Statement(const Connection &connection, const char *sql) : connection(connection) {
		if (sqlite3_prepare_v2(connection.get(), sql, -1, &handle, nullptr) != SQLITE_OK) {
			throw ClinicException(std::string("Database error: ") + sqlite3_errmsg(connection.get()));
		}
	}

	Statement(const Statement &) = delete;
	Statement &operator=(const Statement &) = delete;

	~Statement() {
		sqlite3_finalize(handle);
	}

	void bind(int index, int value) {
		sqlite3_bind_int(handle, index, value);
	}

	void bind(int index, const std::string &value) {
		sqlite3_bind_text(handle, index, value.c_str(), -1, SQLITE_TRANSIENT);
	}

	void execute() {
		if (sqlite3_step(handle) != SQLITE_DONE) {
			throw ClinicException(std::string("Database error while saving: ") + sqlite3_errmsg(connection.get()));
		}
		sqlite3_reset(handle);
		sqlite3_clear_bindings(handle);
	}

	bool nextRow() {
		return sqlite3_step(handle) == SQLITE_ROW;
	}

	int integerColumn(int column) const {
		return sqlite3_column_int(handle, column);
	}

	std::string textColumn(int column) const {
		const unsigned char *text = sqlite3_column_text(handle, column);
		return text ? reinterpret_cast<const char *>(text) : "";
	}

private:
	const Connection &connection;
	sqlite3_stmt *handle = nullptr;
};

void insertPatients(const Connection &connection, const std::vector<PatientRow> &patients) {
	Statement insert(connection, INSERT_PATIENT_SQL);
	for (const auto &row : patients) {
		insert.bind(1, row.id);
		insert.bind(2, row.type);
		insert.bind(3, row.name);
		insert.bind(4, row.age);
		insert.bind(5, row.contact);
		insert.bind(6, row.detail);
		insert.execute();
	}
}

void insertAppointments(const Connection &connection, const std::vector<AppointmentRow> &appointments) {
	Statement insert(connection, INSERT_APPOINTMENT_SQL);
	for (const auto &row : appointments) {
		insert.bind(1, row.id);
		insert.bind(2, row.patientId);
		insert.bind(3, row.date);
		insert.bind(4, row.status);
		insert.execute();
	}
}

void rollbackQuietly(const Connection &connection) {
	try {
		connection.exec("ROLLBACK;");
	} catch (const ClinicException &) {
	}
}

}

DatabaseManager::DatabaseManager(std::string file) : filename(std::move(file)) {
	initialise();
}

void DatabaseManager::exec(const std::string &sql) const {
	Connection(filename).exec(sql);
}

void DatabaseManager::initialise() const {
	exec(CREATE_TABLES_SQL);
	try {
		exec(ADD_DETAIL_COLUMN_SQL);
	} catch (const ClinicException &) {
	}
}

void DatabaseManager::replace(const std::vector<PatientRow> &patients,
	const std::vector<AppointmentRow> &appointments) const {
	Connection connection(filename);
	connection.exec("PRAGMA foreign_keys = ON; BEGIN TRANSACTION;");
	try {
		connection.exec("DELETE FROM appointments; DELETE FROM patients;");
		insertPatients(connection, patients);
		insertAppointments(connection, appointments);
		connection.exec("COMMIT;");
	} catch (...) {
		rollbackQuietly(connection);
		throw;
	}
}

void DatabaseManager::read(std::vector<PatientRow> &patients, std::vector<AppointmentRow> &appointments) const {
	Connection connection(filename);
	Statement patientQuery(connection, SELECT_PATIENTS_SQL);
	while (patientQuery.nextRow()) {
		patients.push_back({patientQuery.integerColumn(0), patientQuery.textColumn(1), patientQuery.textColumn(2),
			patientQuery.integerColumn(3), patientQuery.textColumn(4), patientQuery.textColumn(5)});
	}
	Statement appointmentQuery(connection, SELECT_APPOINTMENTS_SQL);
	while (appointmentQuery.nextRow()) {
		appointments.push_back({appointmentQuery.integerColumn(0), appointmentQuery.integerColumn(1),
			appointmentQuery.textColumn(2), appointmentQuery.textColumn(3)});
	}
}

}
