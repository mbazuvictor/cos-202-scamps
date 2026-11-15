#pragma once

namespace scapms {
namespace constants {

inline constexpr int MIN_AGE = 0;
inline constexpr int MAX_AGE = 120;
inline constexpr int FIRST_PATIENT_ID = 1000;
inline constexpr int MAX_PATIENT_ID = 999999;
inline constexpr int CACHE_SIZE = 10;
inline constexpr int DEFAULT_WAIT_ALERT_MINUTES = 5;
inline constexpr int DASHBOARD_REFRESH_MS = 10000;
inline constexpr int REMINDER_CHECK_MS = 60000;
inline constexpr int QUEUE_TICK_MS = 1000;

inline constexpr const char *DATABASE_FILE = "scapms.db";
inline constexpr const char *FORBIDDEN_CHARACTERS = ",\"\\\n\r";

inline constexpr const char *STATUS_PENDING = "Pending";
inline constexpr const char *STATUS_COMPLETED = "Completed";
inline constexpr const char *STATUS_CANCELLED = "Cancelled";

inline constexpr const char *CATEGORY_GENERAL = "General";
inline constexpr const char *CATEGORY_EMERGENCY = "Emergency";
inline constexpr const char *CATEGORY_FOLLOW_UP = "FollowUp";

inline constexpr const char *SORT_ID = "id";
inline constexpr const char *SORT_NAME = "name";
inline constexpr const char *SORT_AGE = "age";
inline constexpr const char *SORT_DATE = "date";

inline constexpr const char *ALGORITHM_SELECTION = "selection";
inline constexpr const char *ALGORITHM_MERGE = "merge";

inline constexpr const char *SEARCH_ID = "id";
inline constexpr const char *SEARCH_NAME = "name";
inline constexpr const char *SEARCH_CATEGORY = "category";

inline constexpr const char *CSV_SIGNATURE = "SCAPMS";
inline constexpr int CSV_VERSION = 2;

}
}
