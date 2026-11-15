#include "Diagrams.h"
#include <QFont>
#include <QImage>
#include <QPainter>
#include <QPolygonF>

namespace {

constexpr int LINE = 26;
constexpr int HEADER = 64;
constexpr int PAD = 14;

struct ClassBox {
	QString name;
	QString stereotype;
	bool isAbstract = false;
	QStringList attributes;
	QStringList operations;
	QRectF rect;
};

QRectF place(ClassBox &box, double x, double y, double width) {
	double height = HEADER;
	if (!box.attributes.isEmpty()) {
		height += box.attributes.size() * LINE + PAD;
	}
	height += box.operations.size() * LINE + PAD;
	box.rect = QRectF(x, y, width, height);
	return box.rect;
}

void drawClass(QPainter &p, const ClassBox &box) {
	p.setPen(QPen(QColor("#102a43"), 2));
	p.setBrush(QColor("#ffffff"));
	p.drawRect(box.rect);
	p.setBrush(QColor("#e1effe"));
	p.drawRect(QRectF(box.rect.x(), box.rect.y(), box.rect.width(), HEADER));
	QFont title("DejaVu Sans", 1);
	title.setPixelSize(20);
	title.setBold(true);
	title.setItalic(box.isAbstract);
	QFont body("DejaVu Sans", 1);
	body.setPixelSize(16);
	p.setFont(body);
	p.drawText(QRectF(box.rect.x(), box.rect.y() + 4, box.rect.width(), 22), Qt::AlignCenter, box.stereotype);
	p.setFont(title);
	p.drawText(QRectF(box.rect.x(), box.rect.y() + 26, box.rect.width(), 34), Qt::AlignCenter, box.name);
	p.setFont(body);
	double y = box.rect.y() + HEADER;
	if (!box.attributes.isEmpty()) {
		for (const auto &line : box.attributes) {
			p.drawText(QPointF(box.rect.x() + 12, y + 20), line);
			y += LINE;
		}
		y += PAD;
		p.drawLine(QPointF(box.rect.x(), y - PAD / 2.0 - 3), QPointF(box.rect.right(), y - PAD / 2.0 - 3));
	}
	for (const auto &line : box.operations) {
		QFont font = body;
		font.setItalic(line.contains("{abstract}"));
		p.setFont(font);
		p.drawText(QPointF(box.rect.x() + 12, y + 20), line);
		y += LINE;
	}
}

void hollowTriangle(QPainter &p, QPointF tip, bool pointingUp) {
	double dir = pointingUp ? 1 : -1;
	QPolygonF triangle;
	triangle << tip << QPointF(tip.x() - 13, tip.y() + 24 * dir) << QPointF(tip.x() + 13, tip.y() + 24 * dir);
	p.setBrush(Qt::white);
	p.setPen(QPen(QColor("#102a43"), 2, Qt::SolidLine));
	p.drawPolygon(triangle);
}

void diamond(QPainter &p, QPointF tip, double direction, bool filled) {
	QPolygonF shape;
	shape << tip << QPointF(tip.x() + 14 * direction, tip.y() - 9) << QPointF(tip.x() + 28 * direction, tip.y()) << QPointF(tip.x() + 14 * direction, tip.y() + 9);
	p.setBrush(filled ? QColor("#102a43") : QColor("#ffffff"));
	p.setPen(QPen(QColor("#102a43"), 2));
	p.drawPolygon(shape);
}

void arrowHead(QPainter &p, QPointF tip, QPointF from) {
	QLineF line(tip, from);
	line.setLength(16);
	QLineF left = line;
	left.setAngle(line.angle() + 25);
	QLineF right = line;
	right.setAngle(line.angle() - 25);
	p.setPen(QPen(QColor("#102a43"), 2));
	p.drawLine(tip, left.p2());
	p.drawLine(tip, right.p2());
}

void label(QPainter &p, const QString &text, QPointF at) {
	QFont font("DejaVu Sans", 1);
	font.setPixelSize(16);
	p.setFont(font);
	p.setPen(QColor("#102a43"));
	p.drawText(at, text);
}

void line(QPainter &p, QPointF a, QPointF b, bool dashed = false) {
	p.setPen(QPen(QColor("#102a43"), 2, dashed ? Qt::DashLine : Qt::SolidLine));
	p.drawLine(a, b);
}

void drawNote(QPainter &p, const QRectF &rect, const QStringList &lines) {
	p.setPen(QPen(QColor("#92400e"), 2));
	p.setBrush(QColor("#fef3c7"));
	p.drawRect(rect);
	QFont font("DejaVu Sans", 1);
	font.setPixelSize(16);
	p.setFont(font);
	p.setPen(QColor("#102a43"));
	double y = rect.y() + 26;
	for (const auto &text : lines) {
		p.drawText(QPointF(rect.x() + 14, y), text);
		y += 24;
	}
}

QImage makeCanvas(int width, int height) {
	QImage image(width * 2, height * 2, QImage::Format_ARGB32);
	image.fill(Qt::white);
	return image;
}

void classHierarchy(const QString &path) {
	QImage image = makeCanvas(1700, 1230);
	QPainter p(&image);
	p.scale(2, 2);
	p.setRenderHint(QPainter::Antialiasing);

	ClassBox appointable{"Appointable", "<<interface>>", true, {}, {"+ scheduleAppointment(date): int {abstract}", "+ cancelAppointment(id): bool {abstract}"}, {}};
	ClassBox record{"PatientRecord", "<<abstract>>", true,
		{"- id: int", "- name, contact, detail: string", "- age: int", "- appointments: vector<Appointment>"},
		{"+ category(): string {abstract}", "+ describe(): string {abstract}", "+ priorityLabel(): string {abstract}", "+ detailLabel(): string {abstract}",
		 "+ scheduleAppointment(date): int", "+ cancelAppointment(id): bool", "+ completeAppointment(id): bool", "+ validate(...) : void  {static}"}, {}};
	ClassBox appointment{"Appointment", "", false,
		{"- id: int", "- patientId: int", "- date: string", "- status: string"},
		{"+ isPending(): bool", "+ markCompleted(): void", "+ markCancelled(): void", "+ isValidDate(date): bool {static}"}, {}};
	ClassBox database{"ClinicDatabase", "", false,
		{"- patients: vector<PatientRecord*>"},
		{"+ find(id): PatientRecord*", "+ require(id): PatientRecord&", "+ add(patient): void", "+ erase(id): void"}, {}};
	ClassBox general{"GeneralPatient", "", false, {}, {"+ category(): \"General\"", "+ describe(): \"Normal priority\"", "+ priorityLabel(): \"Normal\"", "+ detailLabel(): \"Reason for visit\""}, {}};
	ClassBox emergency{"EmergencyPatient", "", false, {}, {"+ category(): \"Emergency\"", "+ describe(): \"HIGH PRIORITY\"", "+ priorityLabel(): \"High Priority\"", "+ detailLabel(): \"Emergency contact\""}, {}};
	ClassBox followUp{"FollowUpPatient", "", false, {}, {"+ category(): \"FollowUp\"", "+ describe(): \"Scheduled follow-up\"", "+ priorityLabel(): \"Scheduled\"", "+ detailLabel(): \"Previous visit date\"", "+ scheduleAppointment(date): int"}, {}};

	place(appointable, 560, 20, 600);
	place(record, 560, 215, 600);
	place(appointment, 20, 250, 400);
	place(database, 1260, 250, 420);
	place(general, 60, 800, 480);
	place(emergency, 610, 800, 480);
	place(followUp, 1160, 800, 480);

	double center = record.rect.center().x();
	double busY = 745;
	line(p, QPointF(center, record.rect.top()), QPointF(center, appointable.rect.bottom() + 24), true);
	hollowTriangle(p, QPointF(center, appointable.rect.bottom()), true);
	line(p, QPointF(center, record.rect.bottom() + 24), QPointF(center, busY));
	line(p, QPointF(general.rect.center().x(), busY), QPointF(followUp.rect.center().x(), busY));
	for (const ClassBox *child : {&general, &emergency, &followUp}) {
		line(p, QPointF(child->rect.center().x(), busY), QPointF(child->rect.center().x(), child->rect.top()));
	}
	hollowTriangle(p, QPointF(center, record.rect.bottom()), true);

	double y = record.rect.top() + 120;
	line(p, QPointF(record.rect.left() - 28, y), QPointF(appointment.rect.right(), y));
	diamond(p, QPointF(record.rect.left(), y), -1, true);
	label(p, "1", QPointF(record.rect.left() + 6, y - 10));
	label(p, "1..*", QPointF(appointment.rect.right() + 6, y - 10));
	label(p, "owns", QPointF(appointment.rect.right() + 36, y + 22));
	line(p, QPointF(database.rect.left() - 28, y), QPointF(record.rect.right(), y));
	diamond(p, QPointF(database.rect.left(), y), -1, true);
	label(p, "1", QPointF(database.rect.left() + 6, y - 12));
	label(p, "0..*", QPointF(record.rect.right() + 6, y - 10));
	label(p, "contains", QPointF(record.rect.right() + 26, y + 22));

	for (const ClassBox *box : {&appointable, &record, &appointment, &database, &general, &emergency, &followUp}) {
		drawClass(p, *box);
	}
	drawNote(p, QRectF(60, 1060, 1580, 130), {
		"Inheritance: GeneralPatient, EmergencyPatient and FollowUpPatient extend the abstract PatientRecord, which implements the Appointable interface.",
		"Abstract classes: Appointable and PatientRecord (pure virtual functions marked {abstract}; italic).",
		"Polymorphism: ClinicManager::describeForConsultation(const PatientRecord&) and the tables call describe() / priorityLabel() through the base type.",
		"Composition / association: each PatientRecord owns 1..* Appointment objects; ClinicDatabase holds the vector of PatientRecord pointers."});
	p.end();
	image.save(path);
}

void chip(QPainter &p, const QRectF &rect, const QString &text, const QColor &fill, bool italic = false) {
	p.setPen(QPen(QColor("#102a43"), 2));
	p.setBrush(fill);
	p.drawRoundedRect(rect, 8, 8);
	QFont font("DejaVu Sans", 1);
	font.setPixelSize(17);
	font.setItalic(italic);
	p.setFont(font);
	p.drawText(rect, Qt::AlignCenter, text);
}

void band(QPainter &p, const QRectF &rect, const QString &title, const QColor &fill) {
	p.setPen(QPen(QColor("#9fb3c8"), 2, Qt::DashLine));
	p.setBrush(fill);
	p.drawRoundedRect(rect, 12, 12);
	QFont font("DejaVu Sans", 1);
	font.setPixelSize(20);
	font.setBold(true);
	p.setFont(font);
	p.setPen(QColor("#102a43"));
	p.drawText(QPointF(rect.x() + 16, rect.y() + 30), title);
}

void layerArrow(QPainter &p, double x, double fromY, double toY, const QString &text) {
	line(p, QPointF(x, fromY), QPointF(x, toY));
	arrowHead(p, QPointF(x, toY), QPointF(x, fromY));
	label(p, text, QPointF(x + 10, (fromY + toY) / 2 + 5));
}

void architecture(const QString &path) {
	QImage image = makeCanvas(1700, 1150);
	QPainter p(&image);
	p.scale(2, 2);
	p.setRenderHint(QPainter::Antialiasing);
	QColor guiFill("#eef2ff"), controllerFill("#ecfdf5"), modelFill("#fff7ed"), utilFill("#f1f5f9"), white("#ffffff");

	band(p, QRectF(20, 10, 1660, 330), "GUI layer (Qt Widgets) - namespace scapms", guiFill);
	chip(p, QRectF(60, 55, 230, 46), "MainWindow", white);
	chip(p, QRectF(735, 55, 230, 46), "Panel {abstract}", white, true);
	chip(p, QRectF(1060, 55, 190, 46), "PatientForm", white);
	chip(p, QRectF(1270, 55, 190, 46), "PatientTable", white);
	chip(p, QRectF(1480, 55, 180, 46), "PatientPicker", white);
	const QStringList panels = {"DashboardPanel", "PatientPanel", "AppointmentPanel", "QueuePanel", "SearchPanel", "ReportsPanel", "AdminPanel"};
	double busY = 190;
	line(p, QPointF(850, 101 + 24), QPointF(850, busY));
	hollowTriangle(p, QPointF(850, 101), true);
	for (int i = 0; i < panels.size(); i++) {
		QRectF rect(40 + i * 235, 230, 220, 50);
		chip(p, rect, panels[i], white);
		line(p, QPointF(rect.center().x(), busY), QPointF(rect.center().x(), rect.top()));
	}
	line(p, QPointF(40 + 110, busY), QPointF(40 + 6 * 235 + 110, busY));
	label(p, "Each panel extends Panel and reports results through signals changed / failed / notified; QTimer drives reminders, queue alerts and dashboard refresh.", QPointF(40, 322));

	band(p, QRectF(20, 440, 1660, 170), "Controller layer - business logic", controllerFill);
	chip(p, QRectF(160, 495, 360, 60), "ClinicManager", white);
	chip(p, QRectF(670, 495, 400, 60), "AppointmentController", white);
	chip(p, QRectF(1220, 495, 360, 60), "SearchEngine", white);
	label(p, "queue, undo stack, 10-slot cache", QPointF(160, 585));
	label(p, "schedule, cancel, complete, reminders, reports", QPointF(670, 585));
	label(p, "linear / binary / recursive search", QPointF(1220, 585));

	band(p, QRectF(20, 710, 1660, 190), "Model layer - namespace scapms", modelFill);
	chip(p, QRectF(60, 760, 250, 50), "Appointable", white, true);
	chip(p, QRectF(350, 760, 270, 50), "PatientRecord", white, true);
	chip(p, QRectF(660, 760, 250, 50), "Appointment", white);
	chip(p, QRectF(950, 760, 270, 50), "ClinicDatabase", white);
	chip(p, QRectF(1260, 760, 250, 50), "PatientFactory", white);
	label(p, "General / Emergency / FollowUp Patient extend PatientRecord; PatientRecord owns 1..* Appointment", QPointF(60, 860));

	band(p, QRectF(20, 990, 1660, 120), "Utilities - namespace scapms::algo and scapms::constants", utilFill);
	chip(p, QRectF(60, 1030, 250, 50), "Algorithms.h", white);
	chip(p, QRectF(350, 1030, 270, 50), "DatabaseManager", white);
	chip(p, QRectF(660, 1030, 250, 50), "FileHandler", white);
	chip(p, QRectF(950, 1030, 250, 50), "IDGenerator", white);
	chip(p, QRectF(1240, 1030, 190, 50), "Exceptions", white);
	chip(p, QRectF(1450, 1030, 200, 50), "Constants", white);

	layerArrow(p, 440, 340, 495, "uses");
	layerArrow(p, 870, 610, 710, "uses");
	layerArrow(p, 1400, 610, 710, "uses");
	layerArrow(p, 1600, 900, 1030, "");
	label(p, "validated and sorted with", QPointF(1330, 960));
	label(p, "SQLite file scapms.db <- DatabaseManager      CSV import/export <- FileHandler", QPointF(60, 1136));
	p.end();
	image.save(path);
}

}

void drawDiagrams(const QString &imageDirectory) {
	classHierarchy(imageDirectory + "/uml-class-hierarchy.png");
	architecture(imageDirectory + "/uml-architecture.png");
}
