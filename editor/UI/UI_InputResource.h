#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_InputResource.h"

#include "ATN/ATN_Network.h"
#include "ATN/ATN_Resources.h"

class UI_InputResource : public QWidget
{
	Q_OBJECT

private:

	ATN::ResourceMarshall *m_argument = nullptr;

	std::vector<int> m_resources;

	static const int STR_FORMAT_BUFF = 1024;

public:
	UI_InputResource(QWidget *parent = Q_NULLPTR);
	~UI_InputResource();

	Ui::UI_InputResource ui;

	// Initializes this input with available resource options and select the current resource marshall
	void initialize(ATN::ResourceMarshall *r, const ATN::Resource *resource, const ATN::Network *net);

public slots:
	void selectResource(int index);

signals:
	void updated();
};
