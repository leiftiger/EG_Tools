#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_NetworkResource.h"

#include "ATN/ATN_Resources.h"

class UI_NetworkResource : public QWidget
{
	Q_OBJECT

public:
	UI_NetworkResource(QWidget *parent = Q_NULLPTR);
	~UI_NetworkResource();

	Ui::UI_NetworkResource ui;

	ATN::Resource *m_resource;
};
