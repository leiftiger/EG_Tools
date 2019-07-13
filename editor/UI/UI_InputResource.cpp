#include "UI_InputResource.h"

UI_InputResource::UI_InputResource(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

UI_InputResource::~UI_InputResource()
{
}

void UI_InputResource::initialize(ATN::ResourceMarshall *argument, const ATN::Resource *resource, const ATN::Network *net)
{
	// Fetch resource name
	ui.label->setText(QString::fromStdString(resource->m_desc + std::string(":")));
	ui.label->setToolTip(ui.label->text());

	ui.comboBox->setToolTip(QString::fromStdString(resource->m_type._to_string()));

	ui.comboBox->clear();

	for (size_t i = 0; i < net->resources().size(); i++)
	{
		ATN::Resource *param = net->resources()[i];

		if (argument->acceptsResourceType(param->m_type))
		{
			char str[STR_FORMAT_BUFF];

			sprintf_s(str, "[%d: %s]", (int)i, param->m_desc.c_str());

			ui.comboBox->addItem(QString::fromStdString(str));

			if (argument->m_value == i)
			{
				ui.comboBox->setCurrentIndex(ui.comboBox->count()-1);
			}
		}
	}

	m_resource = argument;

	// Select the first acceptable resource if we currently don't point to anything
	if (argument->m_value == ATN::ResourceMarshall::INVALID_POINTER && ui.comboBox->count() > 0)
		ui.comboBox->setCurrentIndex(0);
}
