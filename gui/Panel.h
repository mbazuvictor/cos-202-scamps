#pragma once

#include "controller/ClinicManager.h"
#include <QString>
#include <QWidget>
#include <functional>

namespace scapms {

class Panel : public QWidget {
	Q_OBJECT

public:
	virtual void refresh() = 0;

signals:
	void changed(const QString &message);
	void failed(const QString &message);
	void notified(const QString &message);

protected:
	explicit Panel(ClinicManager &manager, QWidget *parent = nullptr) : QWidget(parent), manager(manager) {}
	void run(const std::function<QString()> &operation, bool persist = true);
	void guard(const std::function<void()> &operation);

	ClinicManager &manager;
};

}
