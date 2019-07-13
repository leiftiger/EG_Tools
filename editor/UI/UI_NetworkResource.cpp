#include "UI_NetworkResource.h"

UI_NetworkResource::UI_NetworkResource(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

UI_NetworkResource::~UI_NetworkResource()
{
}

void UI_NetworkResource::setResourceType(const QString &type)
{
	if (m_resource == nullptr)
		return;

	emit changeType(type);
}

void UI_NetworkResource::setResourceInput(bool input)
{
	emit changeInternal(!input);
}

void UI_NetworkResource::setResourceName(const QString &name)
{
	m_resource->m_desc = name.toStdString();

	emit repopulateArguments();
}
