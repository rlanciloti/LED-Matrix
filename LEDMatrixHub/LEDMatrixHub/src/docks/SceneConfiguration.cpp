#include "SceneConfiguration.hpp"

const QFlags<Qt::DockWidgetArea> SceneProperties::DEFAULT_DOCK_AREAS =
	Qt::LeftDockWidgetArea |
	Qt::RightDockWidgetArea |
	Qt::BottomDockWidgetArea;

const QStringList SceneProperties::LED_LAYOUTS = {
	"Snake Right Starting Top Left",
	"Snake Down Starting Top Left",
	"Snake Right Starting Bottom Left",
	"Snake Up Starting Bottom Left",
};

const QStringList SceneProperties::LED_SHAPES = {
	"Square",
	"Cicle"
};

const LED_LAYOUT_DIRECTION SceneProperties::DEFAULT_LED_LAYOUT_DIRECTION = SNAKE_NW_RIGHT;
const LED_SHAPE SceneProperties::DEFAULT_SHAPE = SQUARE;
const int SceneProperties::DEFAULT_WIDTH = 1;
const int SceneProperties::DEFAULT_HEIGHT = 1;
const int SceneProperties::DEFAULT_LED_DISTANCE = 10;

SceneProperties::SceneProperties(QFlags<Qt::DockWidgetArea> allowedArea)
	: AbstractDockWindow("Scene Properties", allowedArea, new QScrollArea()) {
	QDockWidget* dock = getDock();
	
	m_mainWidget->setParent(dock);
	
	container->setLayout(scrollLayout);

	enableConfiguration->setText("Enable Editing");
	enableConfiguration->setChecked(true);

	scrollLayout->addWidget(enableConfiguration);

	setupSizeFrame();
	setupLedLayoutFrame();

	makeConnections();

	((QScrollArea*) m_mainWidget)->setWidget(container);
}

void SceneProperties::setupSizeFrame(void) {
	QFrame* frame = new QFrame();
	QFormLayout* layout = new QFormLayout(frame);

	heightEntry->setMinimum(1);
	heightEntry->setValue(DEFAULT_HEIGHT);
	heightEntry->installEventFilter(scrollEventFilter);
	
	widthEntry->setMinimum(1);
	widthEntry->setValue(DEFAULT_WIDTH);
	widthEntry->installEventFilter(scrollEventFilter);

	layout->addRow("Width:", widthEntry);
	layout->addRow("Height:", heightEntry);

	frame->setFrameShape(QFrame::StyledPanel);
	frame->setFrameShadow(QFrame::Sunken);

	scrollLayout->addWidget(new QLabel("LED Matrix Dimensions"));
	scrollLayout->addWidget(frame);
}

void SceneProperties::setupLedLayoutFrame(void) {
	QFrame* frame = new QFrame();
	QGridLayout* layout = new QGridLayout(frame);

	showLedIds->setText("Show LED Indexes");
	layout->addWidget(showLedIds, 0, 0, 1, 2, Qt::AlignLeft);

	layoutDirection->insertItems(0, LED_LAYOUTS);
	layoutDirection->installEventFilter(scrollEventFilter);
	layoutDirection->setCurrentIndex(DEFAULT_LED_LAYOUT_DIRECTION);
	layout->addWidget(new QLabel("Layout:"), 1, 0);
	layout->addWidget(layoutDirection, 1, 1);

	ledShape->insertItems(0, LED_SHAPES);
	ledShape->installEventFilter(scrollEventFilter);
	ledShape->setCurrentIndex(DEFAULT_SHAPE);
	layout->addWidget(new QLabel("Shape:"), 2, 0);
	layout->addWidget(ledShape, 2, 1);

	ledDistance->setMinimum(0);
	ledDistance->setValue(DEFAULT_LED_DISTANCE);
	ledDistance->installEventFilter(scrollEventFilter);
	layout->addWidget(new QLabel("LED Distance:"), 3, 0);
	layout->addWidget(ledDistance, 3, 1);

	frame->setFrameShape(QFrame::StyledPanel);
	frame->setFrameShadow(QFrame::Sunken);

	scrollLayout->addWidget(new QLabel("LED Layout"));
	scrollLayout->addWidget(frame);
}

void SceneProperties::makeConnections(void) {
	connect(enableConfiguration, &QCheckBox::checkStateChanged, this, [=](Qt::CheckState state) {
		bool enabled = state == Qt::Checked;
		
		setFieldEnabledState(enabled);
	});

	connect(widthEntry, &QSpinBox::valueChanged, this, [=](int width) {
		m_console->Log(STRING("Updating LED Dimensions: {}x{}", width, heightEntry->value()));
		emit matrixDimensionsChanged(width, heightEntry->value());
	});

	connect(heightEntry, &QSpinBox::valueChanged, this, [=](int height) {
		m_console->Log(STRING("Updating LED Dimensions: {}x{}", widthEntry->value(), height));
		emit matrixDimensionsChanged(widthEntry->value(), height);
	});

	connect(layoutDirection, &QComboBox::currentIndexChanged, this, [=](int index) {
		emit ledDirectionChanged((LED_LAYOUT_DIRECTION) index);
	});

	connect(ledShape, &QComboBox::currentIndexChanged, this, [=](int index) {
		emit ledShapeChanged((LED_SHAPE) index);
	});

	connect(ledDistance, &QSpinBox::valueChanged, this, [=](int distance) {
		emit ledDistanceChanged(distance);
	});

	connect(showLedIds, &QCheckBox::checkStateChanged, this, [=](Qt::CheckState state) {
		emit showLedIndexes(state == Qt::Checked);
	});
}

void SceneProperties::AnimationStateChanged(AnimationState state) {
	switch (state) {
		case PAUSED:
			setFieldEnabledState(true);
			break;
		case PLAYING:
			setFieldEnabledState(false);
			break;
	}
}

void SceneProperties::updateWithClient(int height, int width) {
	heightEntry->setValue(height);
	widthEntry->setValue(width);
}

void SceneProperties::setFieldEnabledState(bool enabled) {
	bool enabledChecked = enableConfiguration->checkState() == Qt::Checked;
	bool animationPaused = m_player->GetAnimationState() == AnimationState::PAUSED;
	bool connectedState = m_client->connectedState();
	bool enabledState = enabled && enabledChecked && animationPaused && !connectedState;

	heightEntry->setEnabled(enabledState);
	widthEntry->setEnabled(enabledState);
	layoutDirection->setEnabled(enabledState);
	ledShape->setEnabled(enabledState);
	ledDistance->setEnabled(enabledState);
}
