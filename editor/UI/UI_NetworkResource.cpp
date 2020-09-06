#include "UI_NetworkResource.h"

#include <QAbstractItemView>

UI_NetworkResource::UI_NetworkResource(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.resourceType->view()->setMinimumWidth(ui.resourceType->minimumSizeHint().width());
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
	ui.resourceDesc->setInputError(false);

	if (name.contains('"'))
	{
		ui.resourceDesc->setInputError(true);
		QToolTip::showText(ui.resourceDesc->mapToGlobal(ui.resourceDesc->pos()), tr("Description cannot contain quotes (\")"));

		return;
	}

	m_resource->m_desc = name.toStdString();

	emit repopulateArguments();
}
