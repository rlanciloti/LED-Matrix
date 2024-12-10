#pragma once

#include "common.hpp"

class AbstractDockWindow : public QObject {

public:
	inline QDockWidget* getDock() { return m_dock; }

	QString m_windowTitle;

protected:
	AbstractDockWindow(
		QString title = "",
		QFlags<Qt::DockWidgetArea> allowedArea = Qt::AllDockWidgetAreas,
		QWidget* widget = new QWidget()
	) {
		m_windowTitle = title;
		m_dock->setWindowTitle(title);
		m_dock->setAllowedAreas(allowedArea);
		m_dock->setWidget(widget);
		
		m_mainWidget = widget;
		m_mainLayout = widget->layout();
	}

	inline void setLayout(QLayout* layout) { m_mainWidget->setLayout(layout); }
	inline QLayout* getLayout() { return m_mainWidget->layout(); }

	QLayout* m_mainLayout;
	QWidget* m_mainWidget;

private:
	QDockWidget* m_dock = new QDockWidget();
};
