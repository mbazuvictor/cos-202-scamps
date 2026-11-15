#include "gui/MainWindow.h"
#include "gui/Theme.h"
#include "gui/Widgets.h"
#include <QAction>
#include <QApplication>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTabBar>
#include <QVBoxLayout>

namespace scapms {

namespace {

QAction *addMenuAction(QMenu *menu, const QString &text, const QKeySequence &shortcut, QObject *receiver,
	const char *slot) {
	QAction *action = menu->addAction(text);
	action->setShortcut(shortcut);
	QObject::connect(action, SIGNAL(triggered()), receiver, slot);
	return action;
}

}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	setWindowTitle("SCAPMS | Smart Clinic Management");
	resize(1180, 760);
	setStyleSheet(theme::APPLICATION_STYLE);

	pages = new QStackedWidget;
	pages->addWidget(makeWelcomePage());
	pages->addWidget(makeWorkspacePage());
	setCentralWidget(pages);

	status = new QLabel("Ready");
	summary = new QLabel;
	statusBar()->addWidget(status, 1);
	statusBar()->addPermanentWidget(summary);
	buildMenus();

	try {
		manager.load();
	} catch (const std::exception &) {
	}
	refreshAll();
	dashboard->checkReminders();
}

QWidget *MainWindow::makeWelcomePage() {
	auto *page = new QWidget;
	auto *layout = new QVBoxLayout(page);
	layout->setContentsMargins(120, 90, 120, 90);
	auto *title = new QLabel("Welcome to SCAPMS");
	title->setStyleSheet("font-size:34px;font-weight:700;color:#0f172a");
	auto *text = new QLabel("A calm workspace for managing patients, appointments, and the waiting queue.");
	text->setWordWrap(true);
	text->setStyleSheet("font-size:17px;color:#475569;padding:12px 0 28px");
	auto *enter = makeButton("Enter clinic workspace", "Open the clinic dashboard");
	enter->setMinimumHeight(48);
	connect(enter, &QPushButton::clicked, this, &MainWindow::enterClinic);
	layout->addStretch();
	layout->addWidget(title);
	layout->addWidget(text);
	layout->addWidget(enter, 0, Qt::AlignLeft);
	layout->addStretch();
	return page;
}

QWidget *MainWindow::makeWorkspacePage() {
	auto *page = new QWidget;
	auto *root = new QVBoxLayout(page);
	auto *header = new QHBoxLayout;
	auto *title = new QLabel("Clinic workspace");
	title->setStyleSheet("font-size:24px;font-weight:700;color:#0f2a43;padding:12px 4px");
	auto *back = makeButton("Back to welcome", "Return to the welcome screen");
	connect(back, &QPushButton::clicked, this, &MainWindow::leaveClinic);
	header->addWidget(title);
	header->addStretch();
	header->addWidget(back);
	root->addLayout(header);

	tabs = new QTabWidget;
	tabs->tabBar()->setCursor(Qt::PointingHandCursor);
	dashboard = new DashboardPanel(manager);
	patientPanel = new PatientPanel(manager);
	searchPanel = new SearchPanel(manager);
	adminPanel = new AdminPanel(manager);
	addPanel(dashboard, "Dashboard");
	addPanel(patientPanel, "Patients");
	addPanel(new AppointmentPanel(manager), "Appointments");
	addPanel(new QueuePanel(manager), "Queue");
	addPanel(searchPanel, "Search && Sort");
	addPanel(new ReportsPanel(manager), "Reports");
	addPanel(adminPanel, "Admin");
	root->addWidget(tabs);
	return page;
}

void MainWindow::addPanel(Panel *panel, const QString &title) {
	tabs->addTab(panel, title);
	panels.push_back(panel);
	connect(panel, &Panel::changed, this, &MainWindow::showChange);
	connect(panel, &Panel::failed, this, &MainWindow::showError);
	connect(panel, &Panel::notified, this, &MainWindow::showMessage);
}

void MainWindow::buildMenus() {
	auto *file = menuBar()->addMenu("File");
	addMenuAction(file, "New", QKeySequence("Ctrl+N"), adminPanel, SLOT(newDatabase()));
	addMenuAction(file, "Save", QKeySequence("Ctrl+S"), adminPanel, SLOT(saveData()));
	addMenuAction(file, "Load", QKeySequence("Ctrl+L"), adminPanel, SLOT(loadData()));
	file->addSeparator();
	addMenuAction(file, "Import from file...", QKeySequence("Ctrl+I"), adminPanel, SLOT(importFile()));
	addMenuAction(file, "Export to file...", QKeySequence("Ctrl+E"), adminPanel, SLOT(exportFile()));
	file->addSeparator();
	addMenuAction(file, "Exit", QKeySequence("Ctrl+Q"), qApp, SLOT(quit()));

	auto *edit = menuBar()->addMenu("Edit");
	addMenuAction(edit, "Add patient", QKeySequence("Ctrl+Shift+N"), patientPanel, SLOT(addPatient()));
	addMenuAction(edit, "Undo last operation", QKeySequence("Ctrl+Z"), adminPanel, SLOT(undo()));

	auto *search = menuBar()->addMenu("Search");
	addMenuAction(search, "Find patient", QKeySequence("Ctrl+F"), this, SLOT(findPatient()));
}

void MainWindow::enterClinic() {
	pages->setCurrentIndex(1);
	refreshAll();
	showMessage("Clinic workspace ready.");
}

void MainWindow::leaveClinic() {
	pages->setCurrentIndex(0);
	showMessage("Welcome");
}

void MainWindow::findPatient() {
	pages->setCurrentIndex(1);
	tabs->setCurrentWidget(searchPanel);
	searchPanel->focusSearch();
}

void MainWindow::refreshAll() {
	for (auto *panel : panels) {
		panel->refresh();
	}
	summary->setText(QString("Patients: %1  |  Waiting: %2  |  Undo steps: %3")
		.arg(manager.patients().size())
		.arg(manager.queuedPatients().size())
		.arg(manager.undoDepth()));
}

void MainWindow::showMessage(const QString &message) {
	status->setText(message);
}

void MainWindow::showChange(const QString &message) {
	refreshAll();
	showMessage(message);
}

void MainWindow::showError(const QString &message) {
	QMessageBox::warning(this, "SCAPMS", message);
	showMessage(message);
}

}
