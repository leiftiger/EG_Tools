#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_MainWindowSearchResult.h"

#include "UI_MainWindow.h"

class UI_MainWindow;

class UI_MainWindowSearchResult : public QWidget
{
	Q_OBJECT

public:
	UI_MainWindowSearchResult(QWidget *parent = Q_NULLPTR);
	~UI_MainWindowSearchResult();

private:
	Ui::UI_MainWindowSearchResult ui;

	friend UI_MainWindow;
};
