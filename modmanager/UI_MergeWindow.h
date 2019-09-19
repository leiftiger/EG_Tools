#pragma once

#include <QMainWindow>

#include "GeneratedFiles/ui_UI_MergeWindow.h"

class UI_MergeWindow : public QMainWindow
{
	Q_OBJECT

public:
	UI_MergeWindow(QWidget *parent = Q_NULLPTR);
	~UI_MergeWindow();

	Ui::UI_MergeWindow ui;
};
