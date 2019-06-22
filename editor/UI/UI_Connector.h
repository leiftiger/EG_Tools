#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_Connector.h"

class UI_Connector : public QWidget
{
	Q_OBJECT

public:
	UI_Connector(QWidget *parent = Q_NULLPTR);
	~UI_Connector();

private:
	Ui::UI_Connector ui;
};
