#pragma once

#include "model/PatientRecord.h"
#include "utils/Constants.h"
#include <cctype>
#include <string>

namespace scapms {
namespace algo {

inline std::string lower(std::string text) {
	for (auto &character : text) {
		character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
	}
	return text;
}

inline bool precedes(const PatientRecord *a, const PatientRecord *b, const std::string &field) {
	if (field == constants::SORT_NAME) {
		return lower(a->name()) < lower(b->name());
	}
	if (field == constants::SORT_AGE) {
		return a->age() < b->age();
	}
	if (field == constants::SORT_DATE) {
		std::string dateA = a->nextAppointmentDate();
		std::string dateB = b->nextAppointmentDate();
		if (dateA.empty()) {
			return false;
		}
		return dateB.empty() || dateA < dateB;
	}
	return a->id() < b->id();
}

inline void selectionSort(PatientList &list, const std::string &field) {
	for (size_t i = 0; i + 1 < list.size(); i++) {
		size_t smallest = i;
		for (size_t j = i + 1; j < list.size(); j++) {
			if (precedes(list[j], list[smallest], field)) {
				smallest = j;
			}
		}
		PatientRecord *held = list[i];
		list[i] = list[smallest];
		list[smallest] = held;
	}
}

inline void mergeSort(PatientList &list, int left, int right, const std::string &field) {
	if (left >= right) {
		return;
	}
	int middle = (left + right) / 2;
	mergeSort(list, left, middle, field);
	mergeSort(list, middle + 1, right, field);
	PatientList merged;
	int i = left;
	int j = middle + 1;
	while (i <= middle && j <= right) {
		merged.push_back(precedes(list[j], list[i], field) ? list[j++] : list[i++]);
	}
	while (i <= middle) {
		merged.push_back(list[i++]);
	}
	while (j <= right) {
		merged.push_back(list[j++]);
	}
	for (size_t k = 0; k < merged.size(); k++) {
		list[left + k] = merged[k];
	}
}

inline int linearSearchById(const PatientList &list, int id) {
	for (size_t i = 0; i < list.size(); i++) {
		if (list[i]->id() == id) {
			return static_cast<int>(i);
		}
	}
	return -1;
}

inline int binarySearchById(const PatientList &list, int id) {
	int low = 0;
	int high = static_cast<int>(list.size()) - 1;
	while (low <= high) {
		int middle = (low + high) / 2;
		if (list[middle]->id() == id) {
			return middle;
		}
		if (list[middle]->id() < id) {
			low = middle + 1;
		} else {
			high = middle - 1;
		}
	}
	return -1;
}

inline int binarySearchFirstByName(const PatientList &list, const std::string &name) {
	std::string target = lower(name);
	int low = 0;
	int high = static_cast<int>(list.size()) - 1;
	int found = -1;
	while (low <= high) {
		int middle = (low + high) / 2;
		std::string current = lower(list[middle]->name());
		if (current == target) {
			found = middle;
			high = middle - 1;
		} else if (current < target) {
			low = middle + 1;
		} else {
			high = middle - 1;
		}
	}
	return found;
}

inline void recursiveSearchName(const PatientList &list, size_t index, const std::string &keyword,
	PatientList &matches) {
	if (index >= list.size()) {
		return;
	}
	if (lower(list[index]->name()).find(lower(keyword)) != std::string::npos) {
		matches.push_back(list[index]);
	}
	recursiveSearchName(list, index + 1, keyword, matches);
}

}
}
