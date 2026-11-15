#pragma once

#include "gui/Panel.h"
#include "gui/PatientTable.h"
#include <QLabel>
#include <QListWidget>
#include <QSet>
#include <QTimer>

namespace scapms {

class DashboardPanel : public Panel {
	Q_OBJECT

public:
	explicit DashboardPanel(ClinicManager &manager, QWidget *parent = nullptr);
	void refresh() override;

public slots:
	void checkReminders();

private:
	QLabel *patientsValue;
	QLabel *waitingValue;
	QLabel *pendingValue;
	QLabel *todayValue;
	QLabel *undoValue;
	PatientTable *recentTable;
	QListWidget *reminderList;
	QTimer *refreshTimer;
	QTimer *reminderTimer;
	QSet<int> announced;
};

}
