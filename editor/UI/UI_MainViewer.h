#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_MainViewer.h"

class UI_MainViewer : public QWidget
{
	Q_OBJECT

public:
	UI_MainViewer(QWidget *parent = Q_NULLPTR);
	~UI_MainViewer();

private:
	Ui::UI_MainViewer ui;
};
