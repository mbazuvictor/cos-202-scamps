#pragma once

#include <QLabel>
#include <QPushButton>
#include <QString>

namespace scapms {

QPushButton *makeButton(const QString &text, const QString &tooltip);
QLabel *makeHeading(const QString &text);
QLabel *makeHint(const QString &text);

}
