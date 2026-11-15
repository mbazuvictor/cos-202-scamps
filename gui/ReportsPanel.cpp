#include "gui/ReportsPanel.h"
#include "gui/Widgets.h"
#include "utils/Constants.h"
#include <QVBoxLayout>

namespace scapms {

ReportsPanel::ReportsPanel(ClinicManager &manager, QWidget *parent) : Panel(manager, parent) {
	auto *root = new QVBoxLayout(this);
	root->addWidget(makeHeading("Reports"));
	root->addWidget(makeHint("Reports are generated from the live clinic records."));
	view = new QTextEdit;
	view->setReadOnly(true);
	root->addWidget(view);
}

QString ReportsPanel::patientRows(const PatientList &patients, const QString &countHeading, bool completed) const {
	if (patients.empty()) {
		return "<p><i>None.</i></p>";
	}
	QString html = "<table border='1' cellpadding='6' cellspacing='0'>";
	html += "<tr><th>ID</th><th>Name</th><th>" + countHeading + "</th></tr>";
	for (const auto *patient : patients) {
		int count = patient->countStatus(completed ? constants::STATUS_COMPLETED : constants::STATUS_PENDING);
		QString name = QString::fromStdString(patient->name()).toHtmlEscaped();
		html += QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>").arg(patient->id()).arg(name).arg(count);
	}
	return html + "</table>";
}

void ReportsPanel::refresh() {
	const auto &controller = manager.appointmentController();
	AppointmentSummary summary = controller.summary();
	QString html = "<h3>1. Most frequently attended patients</h3>";
	html += patientRows(controller.mostAttended(), "Completed appointments", true);
	html += "<h3>2. Patients with pending appointments</h3>";
	html += patientRows(controller.withPending(), "Pending appointments", false);
	html += QString("<h3>3. Patient category distribution</h3>"
					"<p>General Patients: %1<br>Emergency Patients: %2<br>Follow-Up Patients: %3</p>")
		.arg(manager.countCategory(constants::CATEGORY_GENERAL))
		.arg(manager.countCategory(constants::CATEGORY_EMERGENCY))
		.arg(manager.countCategory(constants::CATEGORY_FOLLOW_UP));
	html += QString("<h3>4. Daily appointment summary</h3>"
					"<p>Total Appointments: %1<br>Completed: %2<br>Pending: %3<br>Cancelled: %4</p>")
		.arg(summary.total).arg(summary.completed).arg(summary.pending).arg(summary.cancelled);
	view->setHtml(html);
}

}
