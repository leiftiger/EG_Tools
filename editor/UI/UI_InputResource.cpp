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

	m_resources.clear();

	for (size_t i = 0; i < net->resources().size(); i++)
	{
		ATN::Resource *param = net->resources()[i];

		if (argument->acceptsResourceType(param->m_type))
		{
			char str[STR_FORMAT_BUFF];

			snprintf(str, STR_FORMAT_BUFF, "[%d: %s]", (int)i, param->m_desc.c_str());

			ui.comboBox->addItem(QString::fromStdString(str));

			if (argument->m_value == (std::int64_t)i)
			{
				ui.comboBox->setCurrentIndex(ui.comboBox->count()-1);
			}

			m_resources.push_back(i);
		}
	}

	m_argument = argument;

	// Select the first acceptable resource if we currently don't point to anything
	if (argument->m_value == ATN::ResourceMarshall::INVALID_POINTER && ui.comboBox->count() > 0)
	{
		ui.comboBox->setCurrentIndex(0);

		// May not be called if the combobox is already at index 0
		selectResource(0);
	}
}

void UI_InputResource::selectResource(int index)
{
	if (m_argument == nullptr)
		return;

	m_argument->m_value = m_resources[index];

	emit updated();
}
