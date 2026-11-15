#pragma once

#include <string>

namespace scapms {

class Appointment {
public:
	Appointment(int id, int patientId, std::string date, std::string status);
	static bool isValidDate(const std::string &date);
	static bool isValidStatus(const std::string &status);
	int id() const { return id_; }
	int patientId() const { return patientId_; }
	const std::string &date() const { return date_; }
	const std::string &status() const { return status_; }
	bool isPending() const;
	bool isCompleted() const;
	bool isCancelled() const;
	void markPending();
	void markCompleted();
	void markCancelled();

private:
	int id_;
	int patientId_;
	std::string date_;
	std::string status_;
};

}
