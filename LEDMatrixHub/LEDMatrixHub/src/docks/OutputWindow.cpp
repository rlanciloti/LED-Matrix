#include "OutputWindow.hpp"

const QFlags<Qt::DockWidgetArea> OutputWindow::DEFAULT_DOCK_AREAS =
	Qt::LeftDockWidgetArea |
	Qt::RightDockWidgetArea |
	Qt::BottomDockWidgetArea;

OutputWindow::OutputWindow(QFlags<Qt::DockWidgetArea> allowedArea) 
	: AbstractDockWindow("Output", allowedArea) {
	m_mainLayout = new QVBoxLayout(m_mainWidget);

	m_textBox->setMinimumHeight(50);
	m_textBox->setLineWrapMode(QTextEdit::NoWrap);
	m_textBox->setReadOnly(true);
	m_textBox->setUndoRedoEnabled(false);
	m_textBox->setFont(DEFAULT_FONT);
	m_textBox->setTextColor(Qt::white);

	m_mainLayout->addWidget(m_textBox);
}

void OutputWindow::Info(const char* text) {
	QColor color = m_textBox->textColor();
	m_textBox->setTextColor(Qt::white);
	WriteLine(text);
	m_textBox->setTextColor(color);
}

void OutputWindow::Error(const char* text) {
	QColor color = m_textBox->textColor();
	m_textBox->setTextColor(Qt::red);
	WriteLine(text);
	m_textBox->setTextColor(color);
}

void OutputWindow::Warning(const char* text) {
	QColor color = m_textBox->textColor();
	m_textBox->setTextColor(Qt::yellow);
	WriteLine(text);
	m_textBox->setTextColor(color);
}

void OutputWindow::Log(const char* text, LOG_TYPE type) {
	auto time = std::chrono::system_clock::now();
	auto t_str = std::format("{:%Y-%m-%d %H:%M:%S}", time);

	switch (type) {
		case ERROR:
			Error(STRING("{} - ERROR: {}", t_str, text));
			break;
		case WARNING:
			Warning(STRING("{} - WARNING: {}", t_str, text));
			break;
		case INFO:
			Info(STRING("{} - INFO: {}", t_str, text));
			break;
	}
}
