#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QThread>

#include <sstream>

#include "GeneratedFiles/ui_UI_MergeWindow.h"

#include "ResourceMerger.h"

#include "UI_MergeWindowWorker.h"

#include <QEvent>
#include "qevent.h"

class UI_MergeWindow : public QMainWindow
{
	Q_OBJECT

protected:

	Ui::UI_MergeWindow ui;

	ResourceMerger *m_merger;
	UI_MergeWindowWorker *m_worker;

	QThread *m_thread;

	bool m_firstStringReceived = false;

	void initialize();

public:
	UI_MergeWindow(QWidget *parent, ResourceMerger *merger);
	~UI_MergeWindow();

public slots:
	void receiveMergeOutput(QString str);
	void receiveMergeProgress(double progress);
};
