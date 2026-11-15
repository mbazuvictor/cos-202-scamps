#include "gui/Panel.h"

namespace scapms {

void Panel::run(const std::function<QString()> &operation, bool persist) {
	try {
		QString message = operation();
		if (persist) {
			manager.save();
		}
		emit changed(message);
	} catch (const std::exception &error) {
		emit failed(error.what());
	}
}

void Panel::guard(const std::function<void()> &operation) {
	try {
		operation();
	} catch (const std::exception &error) {
		emit failed(error.what());
	}
}

}
