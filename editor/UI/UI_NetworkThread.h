#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_NetworkThread.h"

#include "ATN/ATN_Thread.h"

class UI_NetworkThread : public QWidget
{
	Q_OBJECT

public:
	UI_NetworkThread(QWidget *parent = Q_NULLPTR);
	~UI_NetworkThread();

	ATN::Thread *m_thread = nullptr;

	Ui::UI_NetworkThread ui;


public slots:
	void setName(const QString &name);
};
