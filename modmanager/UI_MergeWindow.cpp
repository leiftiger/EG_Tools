#include "UI_MergeWindow.h"

#include "PatchATN.h"
#include "PatchDesc.h"
#include "PatchGeneric.h"
#include "PatchStrings.h"

#include <QPropertyAnimation>
#include <QScrollBar>

void UI_MergeWindow::initialize()
{
	this->setWindowFlag(Qt::WindowCloseButtonHint, false);
	this->setWindowFlag(Qt::WindowMinimizeButtonHint, false);

	ui.progressBar->setValue(0);
	ui.progressBar->setRange(0, 10000);

	m_merger->addPatcher(new PatcherATN());
	m_merger->addPatcher(new PatcherDesc());
	m_merger->addPatcher(new PatcherStrings());

	// Generic text patchers that either isn't handled in detail 
	// or offer no meaningful way to perform extended delta patching
	m_merger->addPatcher(new PatcherGeneric(".ane", false));	// Animation sounds
	m_merger->addPatcher(new PatcherGeneric(".as", false));		// Animation set
	m_merger->addPatcher(new PatcherGeneric(".COL", false));	// Model metadata
	m_merger->addPatcher(new PatcherGeneric(".csb", false));	// UI
	m_merger->addPatcher(new PatcherGeneric(".csv", true));		// Comma separated configs
	m_merger->addPatcher(new PatcherGeneric(".ini", true));		// Configs
	m_merger->addPatcher(new PatcherGeneric(".mus", true));		// Music settings
	m_merger->addPatcher(new PatcherGeneric(".nam", false));	// Agent names
	m_merger->addPatcher(new PatcherGeneric(".pat", false));	// Patterns for construction
	m_merger->addPatcher(new PatcherGeneric(".pap", false));	// Particles
	m_merger->addPatcher(new PatcherGeneric(".spb", true));		// Sound definitions
	m_merger->addPatcher(new PatcherGeneric(".xml", false));	// Glossaries, objective details

	m_thread = new QThread();

	m_worker = new UI_MergeWindowWorker(Q_NULLPTR, *m_merger);

	connect(m_thread, SIGNAL(started()), m_worker, SLOT(performMerging()));

	connect(m_worker, SIGNAL(mergeOutput(QString)), this, SLOT(receiveMergeOutput(QString)));
	connect(m_worker, SIGNAL(mergeProgress(double)), this, SLOT(receiveMergeProgress(double)));

	m_worker->moveToThread(m_thread);

	m_thread->start();
}

UI_MergeWindow::UI_MergeWindow(QWidget *parent, ResourceMerger *merger) : QMainWindow(parent), m_merger(merger)
{
	ui.setupUi(this);

	initialize();
}

UI_MergeWindow::~UI_MergeWindow()
{
	m_thread->quit();

	while (!m_thread->isFinished())
	{
		// Block until it has terminated
	}

	delete m_thread;

	delete m_worker;

	delete m_merger;
}

void UI_MergeWindow::receiveMergeProgress(double progress)
{
	int intProgress = 0;

	intProgress = std::min(ui.progressBar->maximum(), (int)(progress*ui.progressBar->maximum()));

	QPropertyAnimation *anim = new QPropertyAnimation(ui.progressBar, "value");

	anim->setStartValue(ui.progressBar->value());
	anim->setEndValue(intProgress);

	anim->setDuration(500);

	anim->start();

	if (progress > 1)
	{
		this->setWindowFlag(Qt::WindowCloseButtonHint, true);
		// The above causes the window to hide, we show it immediately to prevent that
		this->show();
	}
}

void UI_MergeWindow::receiveMergeOutput(QString str)
{
	ui.textOutput->setPlainText(ui.textOutput->toPlainText() + str);

	// Scroll to bottom
	ui.textOutput->verticalScrollBar()->setValue(ui.textOutput->verticalScrollBar()->maximum());
}
