cmake_minimum_required(VERSION 3.16)
project(SCAPMS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(SQLite3 REQUIRED)

add_library(scapms_core STATIC
    model/Appointment.cpp model/Appointment.h
    model/PatientRecord.cpp model/PatientRecord.h
    model/GeneralPatient.cpp model/GeneralPatient.h
    model/EmergencyPatient.cpp model/EmergencyPatient.h
    model/FollowUpPatient.cpp model/FollowUpPatient.h
    model/PatientDetails.h
    model/PatientFactory.cpp model/PatientFactory.h
    model/ClinicDatabase.cpp model/ClinicDatabase.h
    utils/Algorithms.h utils/Constants.h utils/Exceptions.h utils/Rows.h
    utils/DatabaseManager.cpp utils/DatabaseManager.h
    utils/FileHandler.cpp utils/FileHandler.h
    utils/IDGenerator.cpp utils/IDGenerator.h
)
target_include_directories(scapms_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(scapms_core PRIVATE -Wall -Wextra)
target_link_libraries(scapms_core PUBLIC SQLite::SQLite3)
