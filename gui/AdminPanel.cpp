#include "gui/AdminPanel.h"
#include "gui/Widgets.h"
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>
#include <QVBoxLayout>

namespace scapms {

AdminPanel::AdminPanel(ClinicManager &manager, QWidget *parent) : Panel(manager, parent) {
	auto *root = new QVBoxLayout(this);
	root->addWidget(makeHeading("Administration"));
	root->addWidget(makeHint("Undo the latest delete, edit or cancellation, and move data in and out of the system."));
	undoLabel = new QLabel;
	root->addWidget(undoLabel);
	auto *grid = new QGridLayout;
	undoButton = makeButton("Undo last operation", "Reverse the most recent administrative operation (Ctrl+Z)");
	auto *saveButton = makeButton("Save to database", "Write all records to scapms.db (Ctrl+S)");
	auto *loadButton = makeButton("Load from database", "Replace the current records with the saved database (Ctrl+L)");
	auto *newButton = makeButton("New database", "Clear all records (Ctrl+N)");
	auto *importButton = makeButton("Import from file...", "Choose a SCAPMS CSV file to import");
	auto *exportButton = makeButton("Export to file...", "Save all records to a CSV file you choose");
	grid->addWidget(undoButton, 0, 0);
	grid->addWidget(saveButton, 1, 0);
	grid->addWidget(loadButton, 1, 1);
	grid->addWidget(newButton, 1, 2);
	grid->addWidget(importButton, 2, 0);
	grid->addWidget(exportButton, 2, 1);
	root->addLayout(grid);
	root->addStretch();
	connect(undoButton, &QPushButton::clicked, this, &AdminPanel::undo);
	connect(saveButton, &QPushButton::clicked, this, &AdminPanel::saveData);
	connect(loadButton, &QPushButton::clicked, this, &AdminPanel::loadData);
	connect(newButton, &QPushButton::clicked, this, &AdminPanel::newDatabase);
	connect(importButton, &QPushButton::clicked, this, &AdminPanel::importFile);
	connect(exportButton, &QPushButton::clicked, this, &AdminPanel::exportFile);
}

void AdminPanel::refresh() {
	undoButton->setEnabled(manager.canUndo());
	if (!manager.canUndo()) {
		undoLabel->setText("Nothing to undo.");
		return;
	}
	QString next = QString::fromStdString(manager.nextUndoDescription());
	undoLabel->setText(QString("%1 step(s) can be undone. Next: %2").arg(manager.undoDepth()).arg(next));
}

void AdminPanel::undo() {
	run([&] { return QString::fromStdString(manager.undo()); });
}

void AdminPanel::saveData() {
	run([&] {
		manager.save();
		return QString("Data saved to scapms.db.");
	}, false);
}

void AdminPanel::loadData() {
	run([&] {
		manager.load();
		return QString("Data loaded.");
	}, false);
}

void AdminPanel::newDatabase() {
	if (QMessageBox::question(this, "New database", "Clear all current records?") != QMessageBox::Yes) {
		return;
	}
	run([&] {
		manager.newDatabase();
		return QString("New empty database created.");
	});
}

void AdminPanel::importFile() {
	QString path = QFileDialog::getOpenFileName(this, "Import patient data", QString(),
		"SCAPMS data (*.csv);;All files (*)");
	if (path.isEmpty()) {
		return;
	}
	QString warning = "Importing replaces all current records. Continue?";
	if (QMessageBox::question(this, "Import", warning) != QMessageBox::Yes) {
		return;
	}
	run([&] {
		manager.importFromFile(path.toStdString());
		return "Imported records from " + path;
	});
}

void AdminPanel::exportFile() {
	QString path = QFileDialog::getSaveFileName(this, "Export patient data", "scapms-export.csv",
		"SCAPMS data (*.csv)");
	if (path.isEmpty()) {
		return;
	}
	run([&] {
		manager.exportToFile(path.toStdString());
		return "Exported records to " + path;
	}, false);
}

}
