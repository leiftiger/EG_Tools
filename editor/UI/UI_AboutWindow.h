#pragma once

#include <QMainWindow>
#include "GeneratedFiles/ui_UI_AboutWindow.h"

class UI_AboutWindow : public QMainWindow
{
	Q_OBJECT

public:
	UI_AboutWindow(QWidget *parent = Q_NULLPTR);
	~UI_AboutWindow();

private:
	Ui::UI_AboutWindow ui;
};
