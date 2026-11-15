#pragma once

#include "gui/Panel.h"
#include <QTextEdit>

namespace scapms {

class ReportsPanel : public Panel {
	Q_OBJECT

public:
	explicit ReportsPanel(ClinicManager &manager, QWidget *parent = nullptr);
	void refresh() override;

private:
	QString patientRows(const PatientList &patients, const QString &countHeading, bool completed) const;

	QTextEdit *view;
};

}
