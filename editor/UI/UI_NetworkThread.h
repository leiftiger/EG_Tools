#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_NetworkThread.h"

class UI_NetworkThread : public QWidget
{
	Q_OBJECT

public:
	UI_NetworkThread(QWidget *parent = Q_NULLPTR);
	~UI_NetworkThread();

private:
	Ui::UI_NetworkThread ui;
};
