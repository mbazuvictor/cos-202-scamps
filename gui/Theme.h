#pragma once

namespace scapms {
namespace theme {

inline constexpr const char *EMERGENCY_BACKGROUND = "#fde8e8";
inline constexpr const char *EMERGENCY_TEXT = "#9b1c1c";
inline constexpr const char *GENERAL_BACKGROUND = "#ffffff";
inline constexpr const char *GENERAL_TEXT = "#102a43";
inline constexpr const char *FOLLOW_UP_BACKGROUND = "#e1effe";
inline constexpr const char *FOLLOW_UP_TEXT = "#1e429f";

inline constexpr const char *PENDING_BACKGROUND = "#fef3c7";
inline constexpr const char *PENDING_TEXT = "#92400e";
inline constexpr const char *COMPLETED_BACKGROUND = "#def7ec";
inline constexpr const char *COMPLETED_TEXT = "#03543f";
inline constexpr const char *CANCELLED_BACKGROUND = "#f3f4f6";
inline constexpr const char *CANCELLED_TEXT = "#6b7280";

inline constexpr const char *ERROR_STYLE = "color:#c53030;font-weight:600";
inline constexpr const char *WARNING_STYLE = "color:#b45309;font-weight:600";
inline constexpr const char *HINT_STYLE = "color:#486581";
inline constexpr const char *HEADING_STYLE = "font-size:20px;font-weight:600;color:#102a43";
inline constexpr const char *DELETE_BUTTON_STYLE =
	"QPushButton{background:#c53030;color:#ffffff} QPushButton:hover{background:#9b2c2c}";
inline constexpr const char *CARD_STYLE = "QFrame#card{background:#f5f7fa;border:1px solid #d9e2ec;border-radius:8px}";

inline constexpr const char *APPLICATION_STYLE =
	"QMainWindow{background:#f4f6f9;color:#102a43} QWidget{font-size:14px;color:#102a43} "
	"QDialog{background:#ffffff;color:#102a43} QDialog QLabel{color:#102a43;background:#ffffff} "
	"QTabWidget::pane{border:1px solid #d9e2ec;background:#ffffff} "
	"QTabBar::tab{background:#d9e2ec;color:#243b53;padding:10px 18px;margin-right:2px} "
	"QTabBar::tab:selected{background:#102a43;color:#ffffff} "
	"QPushButton{padding:8px 14px;border:0;border-radius:5px;background:#102a43;color:#ffffff;font-weight:600} "
	"QPushButton:hover{background:#243b53} QPushButton:disabled{background:#bcccdc;color:#627d98} "
	"QDialog QPushButton{min-width:90px} "
	"QLineEdit,QComboBox,QSpinBox,QDateEdit{padding:7px;border:1px solid #9fb3c8;border-radius:4px;"
	"background:#ffffff;color:#102a43;selection-background-color:#486581} "
	"QDateEdit QLineEdit{background:#ffffff;color:#102a43;border:0} "
	"QDateEdit::drop-down{width:30px;background:#e6eef5;border-left:1px solid #9fb3c8} "
	"QCalendarWidget{background:#ffffff;color:#102a43} "
	"QCalendarWidget QAbstractItemView{background:#ffffff;color:#102a43;"
	"selection-background-color:#486581;selection-color:#ffffff} "
	"QComboBox QAbstractItemView{background:#ffffff;color:#102a43;selection-background-color:#486581;"
	"selection-color:#ffffff;border:1px solid #9fb3c8} "
	"QTableWidget{background:#ffffff;color:#102a43;border:1px solid #d9e2ec;alternate-background-color:#f5f7fa;"
	"selection-background-color:#243b53;selection-color:#ffffff} "
	"QHeaderView::section{background:#102a43;color:#ffffff;padding:8px;font-weight:bold;border:0} "
	"QListWidget{background:#ffffff;color:#102a43;border:1px solid #d9e2ec} "
	"QStatusBar{background:#102a43;color:#ffffff} QStatusBar QLabel{color:#ffffff} "
	"QTextEdit{background:#ffffff;color:#102a43;border:1px solid #d9e2ec}";

}
}
