#include "gui/SearchPanel.h"
#include "gui/Theme.h"
#include "gui/Widgets.h"
#include "utils/Constants.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace scapms {

namespace {

constexpr int SELECTION_SORT_LIMIT = 50;

const char *SELECTION_SORT_REASON =
	"Selection sort scans for the smallest remaining record and makes at most n-1 swaps. "
	"It is simple and fine for the small lists a clinic usually holds.";

const char *MERGE_SORT_REASON =
	"Merge sort splits the list in half, sorts each half recursively and merges them in O(n log n). "
	"It stays fast on large lists and is stable, so patients with equal values keep their registration order.";

const char *fieldFor(int index) {
	const char *fields[] = {constants::SORT_NAME, constants::SORT_ID, constants::SORT_AGE, constants::SORT_DATE};
	return fields[index];
}

const char *searchModeFor(int index) {
	const char *modes[] = {constants::SEARCH_ID, constants::SEARCH_NAME, constants::SEARCH_CATEGORY};
	return modes[index];
}

}

SearchPanel::SearchPanel(ClinicManager &manager, QWidget *parent) : Panel(manager, parent) {
	auto *root = new QVBoxLayout(this);
	root->addWidget(makeHeading("Search and sort"));

	auto *searchRow = new QHBoxLayout;
	searchMode = new QComboBox;
	searchMode->addItems({"ID", "Name", "Category"});
	searchMode->setToolTip("Choose what to search by. The algorithm is picked from the state of the list.");
	searchEdit = new QLineEdit;
	searchEdit->setPlaceholderText("Type to search (Ctrl+F)");
	searchEdit->setClearButtonEnabled(true);
	searchEdit->setToolTip("Results update as you type");
	auto *searchButton = makeButton("Search", "Run the search now");
	searchRow->addWidget(searchMode);
	searchRow->addWidget(searchEdit, 1);
	searchRow->addWidget(searchButton);
	root->addLayout(searchRow);
	searchStatus = makeHint("");
	root->addWidget(searchStatus);

	auto *sortRow = new QHBoxLayout;
	sortAlgorithm = new QComboBox;
	sortAlgorithm->addItems({"Selection Sort", "Merge Sort"});
	sortAlgorithm->setToolTip("Choose the sorting algorithm");
	sortField = new QComboBox;
	sortField->addItems({"Name", "Patient ID", "Age", "Appointment date"});
	sortField->setToolTip("Choose which field to sort by");
	auto *sortButton = makeButton("SORT", "Sort all patient records with the chosen algorithm");
	sortRow->addWidget(new QLabel("Sort algorithm"));
	sortRow->addWidget(sortAlgorithm);
	sortRow->addWidget(new QLabel("Sort by"));
	sortRow->addWidget(sortField);
	sortRow->addWidget(sortButton);
	sortRow->addStretch();
	root->addLayout(sortRow);
	orderLabel = makeHint("");
	explanation = makeHint("");
	root->addWidget(orderLabel);
	root->addWidget(explanation);

	table = new PatientTable;
	root->addWidget(table);

	connect(searchButton, &QPushButton::clicked, this, &SearchPanel::searchNow);
	connect(searchEdit, &QLineEdit::returnPressed, this, &SearchPanel::searchNow);
	connect(searchEdit, &QLineEdit::textChanged, this, &SearchPanel::searchWhileTyping);
	connect(searchMode, &QComboBox::currentIndexChanged, this, &SearchPanel::searchWhileTyping);
	connect(sortButton, &QPushButton::clicked, this, &SearchPanel::sortNow);
	connect(sortAlgorithm, &QComboBox::currentIndexChanged, this, &SearchPanel::explainAlgorithm);
	explainAlgorithm();
}

void SearchPanel::focusSearch() {
	searchEdit->setFocus();
	searchEdit->selectAll();
}

QString SearchPanel::orderText() const {
	if (manager.sortedBy().empty()) {
		return "Current order: registration order (unsorted). ID search uses Linear Search.";
	}
	QString field = QString::fromStdString(manager.sortedBy());
	QString searchHint;
	if (manager.sortedBy() == constants::SORT_ID) {
		searchHint = "ID search uses Binary Search.";
	} else if (manager.sortedBy() == constants::SORT_NAME) {
		searchHint = "Exact name search uses Binary Search.";
	}
	return "Current order: sorted by " + field + ". " + searchHint;
}

void SearchPanel::explainAlgorithm() {
	int count = static_cast<int>(manager.patients().size());
	bool selection = sortAlgorithm->currentIndex() == 0;
	QString reason = selection ? SELECTION_SORT_REASON : MERGE_SORT_REASON;
	QString better = count <= SELECTION_SORT_LIMIT ? "selection sort" : "merge sort";
	QString advice = QString(" With %1 patients, %2 is the better fit.").arg(count).arg(better);
	explanation->setText(reason + advice);
}

void SearchPanel::showAll() {
	table->setPatients(manager.patients());
	searchStatus->setStyleSheet(theme::HINT_STYLE);
	searchStatus->setText(QString("Showing all %1 patients.").arg(manager.patients().size()));
}

void SearchPanel::runSearch() {
	auto mode = searchModeFor(searchMode->currentIndex());
	SearchResult result = manager.search(mode, searchEdit->text().toStdString());
	table->setPatients(result.patients);
	searchStatus->setStyleSheet(theme::HINT_STYLE);
	QString algorithm = QString::fromStdString(result.algorithm);
	searchStatus->setText(QString("%1 result(s) using %2.").arg(result.patients.size()).arg(algorithm));
}

void SearchPanel::refresh() {
	orderLabel->setText(orderText());
	explainAlgorithm();
	if (searchEdit->text().trimmed().isEmpty()) {
		showAll();
		return;
	}
	searchWhileTyping();
}

void SearchPanel::searchWhileTyping() {
	if (searchEdit->text().trimmed().isEmpty()) {
		showAll();
		return;
	}
	try {
		runSearch();
	} catch (const std::exception &error) {
		searchStatus->setStyleSheet(theme::ERROR_STYLE);
		searchStatus->setText(error.what());
	}
}

void SearchPanel::searchNow() {
	guard([this] { runSearch(); });
	emit notified("Search complete.");
}

void SearchPanel::sortNow() {
	QString algorithm = sortAlgorithm->currentIndex() == 0 ? "selection" : "merge";
	run([&] {
		manager.sort(algorithm.toStdString(), fieldFor(sortField->currentIndex()));
		return QString("Records sorted with %1.").arg(sortAlgorithm->currentText());
	}, false);
}

}
