cmake_minimum_required(VERSION 3.16)
project(SCAPMS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(SQLite3 REQUIRED)
find_package(Qt6 QUIET COMPONENTS Widgets)
if (Qt6_FOUND)
    set(QT_WIDGETS Qt6::Widgets)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets)
    set(QT_WIDGETS Qt5::Widgets)
endif()

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
    controller/AppointmentController.cpp controller/AppointmentController.h
    controller/ClinicManager.cpp controller/ClinicManager.h
    controller/SearchEngine.cpp controller/SearchEngine.h
)
target_include_directories(scapms_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(scapms_core PRIVATE -Wall -Wextra)
target_link_libraries(scapms_core PUBLIC SQLite::SQLite3)

add_executable(scapms
    main.cpp
    gui/Theme.h
    gui/Widgets.cpp gui/Widgets.h
    gui/Panel.cpp gui/Panel.h
    gui/PatientTable.cpp gui/PatientTable.h
    gui/PatientPicker.cpp gui/PatientPicker.h
    gui/PatientForm.cpp gui/PatientForm.h
    gui/PatientPanel.cpp gui/PatientPanel.h
    gui/AppointmentPanel.cpp gui/AppointmentPanel.h
    gui/QueuePanel.cpp gui/QueuePanel.h
    gui/SearchPanel.cpp gui/SearchPanel.h
    gui/ReportsPanel.cpp gui/ReportsPanel.h
    gui/AdminPanel.cpp gui/AdminPanel.h
    gui/DashboardPanel.cpp gui/DashboardPanel.h
    gui/MainWindow.cpp gui/MainWindow.h
)
target_link_libraries(scapms PRIVATE scapms_core ${QT_WIDGETS} SQLite::SQLite3)
target_compile_options(scapms PRIVATE -Wall -Wextra)
