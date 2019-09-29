#pragma once

#include <QMainWindow>
#include "GeneratedFiles/ui_UI_ErrorWindow.h"

#include <string>

class UI_ErrorWindow : public QMainWindow
{
	Q_OBJECT

public:
	UI_ErrorWindow(QWidget *parent = Q_NULLPTR);
	~UI_ErrorWindow();

	void setErrorMessage(const std::string &str);

private:
	Ui::UI_ErrorWindow ui;
};
