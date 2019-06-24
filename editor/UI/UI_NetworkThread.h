#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_NetworkThread.h"

#include "ATN/ATN_Thread.h"

#include <QListWidgetItem>

class UI_NetworkThread : public QWidget
{
	Q_OBJECT

public:
	UI_NetworkThread(QWidget *parent = Q_NULLPTR);
	~UI_NetworkThread();

	ATN::Thread *m_thread;
	QListWidgetItem *m_listItemParent;

	Ui::UI_NetworkThread ui;
};
