# SCAPMS - Smart Clinic Appointment & Patient Management System

SCAPMS gives small clinic staff a single desktop workspace for registering patients, booking appointments, and managing a real-time waiting queue. Staff can track patient categories, undo accidental changes, search and sort records, and export data without juggling spreadsheets. It turns daily clinic operations into a calm, organized workflow.

## System Architecture

The application is built around a clean separation between the Qt-based desktop interface, a central controller for business logic, and a SQLite persistence layer. Patient records, appointments, and the waiting queue live in memory during use and are written to a local database on demand.

```mermaid
flowchart LR
    Staff["Clinic Staff"]
    App["Qt Desktop App"]
    Manager["Clinic Manager"]
    DBManager["Database Manager"]
    DB[("SQLite Database")]

    Staff --> App
    App --> Manager
    Manager --> DBManager
    DBManager --> DB

    style Staff fill:#1e1b4b,stroke:#6366f1,stroke-width:2px,color:#fff
    style App fill:#1e1b4b,stroke:#6366f1,stroke-width:2px,color:#fff
    style Manager fill:#2e1065,stroke:#8b5cf6,stroke-width:2px,color:#fff
    style DBManager fill:#2e1065,stroke:#8b5cf6,stroke-width:2px,color:#fff
    style DB fill:#0f172a,stroke:#3b82f6,stroke-width:2px,color:#fff
```

## Features

- **Patient registration and management**
  Register patients under General, Emergency, or FollowUp categories. Each category carries its own validation rules. For example, follow-up appointments cannot be scheduled before the previous visit date, and emergency cases are highlighted in red.

```mermaid
sequenceDiagram
    actor Staff
    participant Form as "Patient Form"
    participant Manager as "Clinic Manager"
    participant DB as "Database Manager"

    Staff->>Form: Enter patient details
    Form->>Manager: addPatient(type, details)
    Manager->>Manager: Validate input
    Manager->>DB: Save patient record
    DB-->>Manager: Confirm saved
    Manager-->>Form: Return patient ID
    Form-->>Staff: Show updated directory
```

- **Appointment scheduling and tracking**
  Book, complete, or cancel appointments with clear status tracking. Past dates trigger a warning, and cancelled appointments stay visible with strikethrough formatting.

```mermaid
sequenceDiagram
    actor Staff
    participant Panel as "Appointments Panel"
    participant Manager as "Clinic Manager"
    participant Record as "Patient Record"
    participant DB as "Database Manager"

    Staff->>Panel: Select patient and date
    Panel->>Manager: scheduleAppointment(id, date)
    Manager->>Record: scheduleAppointment(date)
    Record-->>Manager: Return appointment ID
    Manager->>DB: Persist appointment
    DB-->>Manager: Confirm saved
    Manager-->>Panel: Refresh appointment list
    Panel-->>Staff: Display scheduled appointment
```

- **Smart search and sorting**
  Search by patient ID, name, or category. The system picks the fastest algorithm based on the current sort state, and staff can sort records with selection or merge sort.

```mermaid
flowchart LR
    Staff["Clinic Staff"] --> Search["Search & Sort Panel"]
    Search --> Mode{"Search mode"}
    Mode -- "ID" --> IDSorted{"List sorted by ID?"}
    Mode -- "Name" --> NameSorted{"List sorted by name?"}
    Mode -- "Category" --> Linear["Linear Search"]
    IDSorted -- "Yes" --> BinaryID["Binary Search"]
    IDSorted -- "No" --> LinearID["Linear Search"]
    NameSorted -- "Yes" --> BinaryName["Binary Search"]
    NameSorted -- "No" --> Recursive["Recursive Search"]

    style Staff fill:#1e1b4b,stroke:#6366f1,stroke-width:2px,color:#fff
    style Search fill:#2e1065,stroke:#8b5cf6,stroke-width:2px,color:#fff
    style Mode fill:#1e1b4b,stroke:#6366f1,stroke-width:2px,color:#fff
    style IDSorted fill:#1e1b4b,stroke:#6366f1,stroke-width:2px,color:#fff
    style NameSorted fill:#1e1b4b,stroke:#6366f1,stroke-width:2px,color:#fff
    style Linear fill:#0f172a,stroke:#3b82f6,stroke-width:2px,color:#fff
    style BinaryID fill:#0f172a,stroke:#3b82f6,stroke-width:2px,color:#fff
    style LinearID fill:#0f172a,stroke:#3b82f6,stroke-width:2px,color:#fff
    style BinaryName fill:#0f172a,stroke:#3b82f6,stroke-width:2px,color:#fff
    style Recursive fill:#0f172a,stroke:#3b82f6,stroke-width:2px,color:#fff
```

- **Real-time waiting queue**
  Add patients to a live queue, call the next person, and get alerts when someone has waited longer than a configured threshold.

- **Reports and analytics dashboard**
  View patient counts, appointment summaries, category distribution, and today's reminders from a central dashboard that refreshes automatically.

- **Undo and data portability**
  Reverse deletes, edits, or cancellations from an undo stack. Save to SQLite, import or export SCAPMS-formatted CSV files, or start a fresh database.

## Installation

### 1. Clone the repository

```bash
git clone https://github.com/mbazuvictor/cos-202-scamps.git
cd cos-202-scamps
```

### 2. Install dependencies (Ubuntu / Debian)

```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev libsqlite3-dev
```

Qt 5 is also supported if Qt 6 is not available.

### 3. Build the project

```bash
cmake -S . -B build
cmake --build build
```

### 4. Run

```bash
./build/scapms
```

The app creates `scapms.db` in the working directory on first launch.

## Usage

When you open SCAPMS, you land on a welcome screen. Click **Enter clinic workspace** to open the main tabs.

Common workflows:

- **Add a patient**: go to the Patients tab, click **Add patient**, fill the form, and save.
- **Schedule an appointment**: go to the Appointments tab, select a patient, pick a date, and click **Schedule appointment**.
- **Manage the queue**: go to the Queue tab, add patients to the queue, then click **Call next patient**.
- **Search or sort records**: use the search box and sort controls to find patients quickly.
- **Undo a mistake**: press `Ctrl+Z` or use Edit > Undo to reverse the last delete or edit.

Keyboard shortcuts:

- `Ctrl+N`: New database
- `Ctrl+S`: Save
- `Ctrl+L`: Load
- `Ctrl+F`: Search
- `Ctrl+I` / `Ctrl+E`: Import / export a CSV file
- `Ctrl+Shift+N`: Add patient
- `Ctrl+Z`: Undo
- `Ctrl+Q`: Exit

## Technologies Used

| Technology | Purpose |
|------------|---------|
| [C++17](https://isocpp.org/) | Core application language |
| [Qt Widgets](https://www.qt.io/) | Desktop GUI framework |
| [SQLite3](https://www.sqlite.org/) | Local data storage |
| [CMake](https://cmake.org/) | Build system |

## Author Info

Built by **Mbazu Onyedikachi Victor**.

## Badges

[![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![Qt](https://img.shields.io/badge/Qt-41CD52?style=for-the-badge&logo=qt&logoColor=white)](https://www.qt.io/)
[![SQLite](https://img.shields.io/badge/SQLite-003B57?style=for-the-badge&logo=sqlite&logoColor=white)](https://www.sqlite.org/)
[![CMake](https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org/)

[![Readme was generated by Dokugen](https://img.shields.io/badge/Readme%20was%20generated%20by-Dokugen-brightgreen)](https://dokugen.samueltuoyo.com)