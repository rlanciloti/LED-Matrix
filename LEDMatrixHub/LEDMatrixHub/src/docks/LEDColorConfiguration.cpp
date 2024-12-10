#include "LEDColorConfiguration.hpp"

const QFlags<Qt::DockWidgetArea> LEDColorConfiguration::DEFAULT_DOCK_AREAS =
	Qt::LeftDockWidgetArea |
	Qt::RightDockWidgetArea |
	Qt::BottomDockWidgetArea;

LEDColorConfiguration::LEDColorConfiguration(QFlags<Qt::DockWidgetArea> allowedArea) 
	: AbstractDockWindow("LED Color Configuration", allowedArea, new QScrollArea()) {
	QDockWidget* dock = getDock();
	m_colorDialog = new QColorDialog();
	m_colorDialog->setCurrentColor(Qt::black);

	m_showColorDialog = new QPushButton();
	m_showColorDialog->setText("");
	m_showColorDialog->setAutoFillBackground(true);

	m_buttonPalette = m_showColorDialog->palette();

	m_mainWidget->setParent(dock);

	m_container->setLayout(m_gridLayout);

	m_gridLayout->addWidget(new QLabel("Color:"), 0, 0);
	m_gridLayout->addWidget(m_showColorDialog, 0, 1);

	m_gridLayout->setRowMinimumHeight(1, 10);

	m_gridLayout->addWidget(new QLabel("Red:"), 2, 0);
	m_gridLayout->addWidget(new QLabel("Green:"), 3, 0);
	m_gridLayout->addWidget(new QLabel("Blue:"), 4, 0);
	
	m_gridLayout->setRowMinimumHeight(5, 10);

	m_gridLayout->addWidget(new QLabel("Hue:"), 6, 0);
	m_gridLayout->addWidget(new QLabel("Saturation:"), 7, 0);
	m_gridLayout->addWidget(new QLabel("Value:"), 8, 0);

	m_gridLayout->addWidget(m_red, 2, 1);
	m_red->setMinimum(0);
	m_red->setMaximum(255);
	
	m_gridLayout->addWidget(m_green, 3, 1);
	m_green->setMinimum(0);
	m_green->setMaximum(255);
	
	m_gridLayout->addWidget(m_blue, 4, 1);
	m_blue->setMinimum(0);
	m_blue->setMaximum(255);
	
	m_gridLayout->addWidget(m_hue, 6, 1);
	m_hue->setMinimum(0);
	m_hue->setMaximum(359);

	m_gridLayout->addWidget(m_saturation, 7, 1);
	m_saturation->setMinimum(0);
	m_saturation->setMaximum(255);

	m_gridLayout->addWidget(m_value, 8, 1);
	m_value->setMinimum(0);
	m_value->setMaximum(255);

	setupConnections();
	((QScrollArea*) m_mainWidget)->setWidget(m_container);
}

void LEDColorConfiguration::setupConnections() {
	connect(m_showColorDialog, &QPushButton::clicked, this, [=]() {
		m_colorDialog->show();
	});

	connect(m_red, &QSpinBox::valueChanged, this, [=](int value) {
		if (!updatingHSV) {
			updatingRGB = true;
			QColor color = m_colorDialog->currentColor();
			color.setRed(value);
			m_colorDialog->setCurrentColor(color);

			updateHSV(color);
			updatingRGB = false;
			updateButtonColor(color);
			//emit colorChanged(color);
		}
	});

	connect(m_green, &QSpinBox::valueChanged, this, [=](int value) {
		if (!updatingHSV) {
			updatingRGB = true;
			QColor color = m_colorDialog->currentColor();
			color.setGreen(value);
			m_colorDialog->setCurrentColor(color);

			updateHSV(color);
			updatingRGB = false;
			updateButtonColor(color);
			//emit colorChanged(color);
		}
	});

	connect(m_blue, &QSpinBox::valueChanged, this, [=](int value) {
		if (!updatingHSV) {
			updatingRGB = true;
			QColor color = m_colorDialog->currentColor();
			color.setBlue(value);
			m_colorDialog->setCurrentColor(color);

			updateHSV(color);
			updatingRGB = false;
			updateButtonColor(color);
			//emit colorChanged(color);
		}
	});

	connect(m_hue, &QSpinBox::valueChanged, this, [=](int value) {
		if (!updatingRGB) {
			updatingHSV = true;
			QColor color = m_colorDialog->currentColor();
			color.setHsv(value, color.saturation(), color.value());
			m_colorDialog->setCurrentColor(color);

			updateRGB(color);
			updatingHSV = false;
			updateButtonColor(color);
			//emit colorChanged(color);
		}
	});

	connect(m_saturation, &QSpinBox::valueChanged, this, [=](int value) {
		if (!updatingRGB) {
			updatingHSV = true;
			QColor color = m_colorDialog->currentColor();
			color.setHsv(color.hue(), value, color.value());
			m_colorDialog->setCurrentColor(color);

			updateRGB(color);
			updatingHSV = false;
			updateButtonColor(color);
			//emit colorChanged(color);
		}
	});

	connect(m_value, &QSpinBox::valueChanged, this, [=](int value) {
		if (!updatingRGB) {
			updatingHSV = true;
			QColor color = m_colorDialog->currentColor();
			color.setHsv(color.hue(), color.saturation(), value);
			m_colorDialog->setCurrentColor(color);

			updateRGB(color);
			updatingHSV = false;
			updateButtonColor(color);
			//emit colorChanged(color);
		}
	});

	connect(m_colorDialog, &QColorDialog::colorSelected, this, [=](const QColor& color) {
		updateRGB(color);
		updateHSV(color);

		updateButtonColor(color);
		emit colorChanged(color);
	});

	connect(
		AnimationPlayer::getInstance(), &AnimationPlayer::AnimationStateChanged,
		this, &LEDColorConfiguration::AnimationStateChanged
	);
}

void LEDColorConfiguration::AnimationStateChanged(AnimationState state) {
	bool enabled = state == AnimationState::PAUSED;

	m_showColorDialog->setEnabled(enabled);
	m_red->setEnabled(enabled);
	m_green->setEnabled(enabled);
	m_blue->setEnabled(enabled);
	m_hue->setEnabled(enabled);
	m_saturation->setEnabled(enabled);
	m_value->setEnabled(enabled);
}
