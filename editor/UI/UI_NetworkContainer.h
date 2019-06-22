#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_NetworkContainer.h"

class UI_NetworkContainer : public QWidget
{
	Q_OBJECT

public:
	UI_NetworkContainer(QWidget *parent = Q_NULLPTR);
	~UI_NetworkContainer();

private:
	Ui::UI_NetworkContainer ui;
};
