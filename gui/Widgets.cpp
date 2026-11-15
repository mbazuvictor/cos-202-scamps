#include "gui/Widgets.h"
#include "gui/Theme.h"

namespace scapms {

QPushButton *makeButton(const QString &text, const QString &tooltip) {
	auto *button = new QPushButton(text);
	button->setToolTip(tooltip);
	button->setCursor(Qt::PointingHandCursor);
	return button;
}

QLabel *makeHeading(const QString &text) {
	auto *label = new QLabel(text);
	label->setStyleSheet(theme::HEADING_STYLE);
	return label;
}

QLabel *makeHint(const QString &text) {
	auto *label = new QLabel(text);
	label->setStyleSheet(theme::HINT_STYLE);
	label->setWordWrap(true);
	return label;
}

}
