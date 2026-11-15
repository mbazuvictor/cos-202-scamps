#pragma once

#include "gui/Panel.h"
#include <QLabel>
#include <QPushButton>

namespace scapms {

class AdminPanel : public Panel {
	Q_OBJECT

public:
	explicit AdminPanel(ClinicManager &manager, QWidget *parent = nullptr);
	void refresh() override;

public slots:
	void undo();
	void saveData();
	void loadData();
	void newDatabase();
	void importFile();
	void exportFile();

private:
	QLabel *undoLabel;
	QPushButton *undoButton;
};

}
