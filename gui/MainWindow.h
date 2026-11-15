#pragma once

#include "controller/ClinicManager.h"
#include "gui/AdminPanel.h"
#include "gui/AppointmentPanel.h"
#include "gui/DashboardPanel.h"
#include "gui/PatientPanel.h"
#include "gui/QueuePanel.h"
#include "gui/ReportsPanel.h"
#include "gui/SearchPanel.h"
#include <QLabel>
#include <QMainWindow>
#include <QStackedWidget>
#include <QTabWidget>
#include <vector>

namespace scapms {

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);

private slots:
	void enterClinic();
	void leaveClinic();
	void findPatient();
	void showMessage(const QString &message);
	void showChange(const QString &message);
	void showError(const QString &message);

private:
	QWidget *makeWelcomePage();
	QWidget *makeWorkspacePage();
	void buildMenus();
	void addPanel(Panel *panel, const QString &title);
	void refreshAll();

	ClinicManager manager;
	QStackedWidget *pages;
	QTabWidget *tabs;
	QLabel *status;
	QLabel *summary;
	std::vector<Panel *> panels;
	DashboardPanel *dashboard;
	PatientPanel *patientPanel;
	SearchPanel *searchPanel;
	AdminPanel *adminPanel;
};

}
