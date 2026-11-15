#include "model/Appointment.h"
#include "utils/Constants.h"
#include "utils/Exceptions.h"
#include <cctype>
#include <utility>

namespace scapms {

namespace {

bool isLeapYear(int year) {
	return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

int daysInMonth(int year, int month) {
	static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	return month == 2 && isLeapYear(year) ? 29 : days[month - 1];
}

}

Appointment::Appointment(int id, int patientId, std::string date, std::string status)
	: id_(id), patientId_(patientId), date_(std::move(date)), status_(std::move(status)) {}

bool Appointment::isValidDate(const std::string &date) {
	if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
		return false;
	}
	for (size_t i = 0; i < date.size(); i++) {
		if (i != 4 && i != 7 && !std::isdigit(static_cast<unsigned char>(date[i]))) {
			return false;
		}
	}
	int year = std::stoi(date.substr(0, 4));
	int month = std::stoi(date.substr(5, 2));
	int day = std::stoi(date.substr(8, 2));
	return month >= 1 && month <= 12 && day >= 1 && day <= daysInMonth(year, month);
}

bool Appointment::isValidStatus(const std::string &status) {
	return status == constants::STATUS_PENDING || status == constants::STATUS_COMPLETED ||
		status == constants::STATUS_CANCELLED;
}

bool Appointment::isPending() const { return status_ == constants::STATUS_PENDING; }
bool Appointment::isCompleted() const { return status_ == constants::STATUS_COMPLETED; }
bool Appointment::isCancelled() const { return status_ == constants::STATUS_CANCELLED; }
void Appointment::markPending() { status_ = constants::STATUS_PENDING; }
void Appointment::markCompleted() { status_ = constants::STATUS_COMPLETED; }
void Appointment::markCancelled() { status_ = constants::STATUS_CANCELLED; }

}
