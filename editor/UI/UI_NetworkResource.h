#pragma once

#include <QWidget>
#include <QToolTip>
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

public slots:

	void setResourceType(const QString &type);
	void setResourceInput(bool input);
	void setResourceName(const QString &name);

signals:
	void repopulateArguments();
	void changeInternal(bool);
	void changeType(const QString &type);
};
