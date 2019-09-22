#include "UI_MergeWindow.h"

#include "PatchATN.h"
#include "PatchDesc.h"

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
