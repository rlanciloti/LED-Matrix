#pragma once

#include <format>
#include <chrono>
#include <iomanip>

#include "AbstractDockWindow.hpp"

#define STRING(fmt, ...) std::format(fmt, __VA_ARGS__).c_str()

enum LOG_TYPE {
	ERROR = 0,
	WARNING = 1,
	INFO = 2
};

class OutputWindow : public AbstractDockWindow {

public:
	static OutputWindow* getInstance() { return getInstance(DEFAULT_DOCK_AREAS); }
	static OutputWindow* getInstance(QFlags<Qt::DockWidgetArea> allowedArea) {
		static OutputWindow* m_instance = new OutputWindow(allowedArea);
		return m_instance;
	}

	inline void WriteLine(const char* text) {
		m_textBox->append(QString(text));
	}

	void Info(const char* text);

	void Error(const char* text);

	void Warning(const char* text);

	void Log(const char* text, LOG_TYPE type = INFO);

	inline void ClearOutput(void) { m_textBox->clear(); }
	
	inline void SetFont(QFont font) { m_textBox->setFont(font); }
	inline void SetTextColor(QColor color) { m_textBox->setTextColor(color); }

private:
	OutputWindow(QFlags<Qt::DockWidgetArea> allowedArea);
	
	static const QFlags<Qt::DockWidgetArea> DEFAULT_DOCK_AREAS;

	const QFont DEFAULT_FONT = QFont("Consolas", 10);
	
	QTextEdit* m_textBox = new QTextEdit();
};
