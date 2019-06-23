#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_NetworkState.h"
#include <QStyle>

class UI_NetworkState : public QWidget
{
	Q_OBJECT

public:
	UI_NetworkState(QWidget *parent = Q_NULLPTR);
	~UI_NetworkState();

private:
	Ui::UI_NetworkState ui;
};
