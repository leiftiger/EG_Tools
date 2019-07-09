#include "UI_NetworkTransition.h"

UI_NetworkTransition::UI_NetworkTransition(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_connector = new UI_ConnectorStart(this);
	m_connector->setFixedSize(21, 21);
}

UI_NetworkTransition::~UI_NetworkTransition()
{
}

void UI_NetworkTransition::initialize(ATN::Transition *transition, ATN::Network *network)
{
	m_transition = transition;
	m_network = network;
}

std::string UI_NetworkTransition::translateParameter(const ATN::ParameterMarshall *paramMarshall, const ATN::Parameter *parameter) const
{
	if (paramMarshall->m_type == ATN::ParameterMarshallType::ParameterIndex)
	{
		char str[STR_FORMAT_BUFF];

		int index = paramMarshall->m_value;

		sprintf_s(str, "[%d: %s]", index, m_network->parameters()[index]->m_desc.c_str());

		return str;
	}

	return parameter->translateValue(paramMarshall->m_value);
}

std::string UI_NetworkTransition::translateResource(const ATN::ResourceMarshall *resourceMarshall, const ATN::Resource *resource) const
{
	char str[STR_FORMAT_BUFF];

	int index = resourceMarshall->m_value;

	sprintf_s(str, "[%d: %s]", index, m_network->resources()[index]->m_desc.c_str());

	return str;
}

std::string UI_NetworkTransition::parseParams(const std::string &format, const std::vector<ATN::ParameterMarshall*> &list, const ATN::IResourceHolder *paramDefiner) const
{
	const std::regex regStr("::param([0-9]+)::");

	std::smatch matches;

	std::string strOut = "";

	std::string strIn = format;

	while(std::regex_search(strIn, matches, regStr))
	{
		strOut += matches.prefix();

		int index = std::stoi(matches[1]);

		strOut += translateParameter(list[index], paramDefiner->parameters()[index]);

		strIn = matches.suffix();
	}

	strOut += strIn;

	return strOut;
}

std::string UI_NetworkTransition::parseResources(const std::string &format, const std::vector<ATN::ResourceMarshall*> &list, const ATN::IResourceHolder *paramDefiner) const
{
	const std::regex regStr("::resource([0-9]+)::");

	std::smatch matches;

	std::string strOut = "";

	std::string strIn = format;

	while (std::regex_search(strIn, matches, regStr))
	{
		strOut += matches.prefix();

		int index = std::stoi(matches[1]);

		strOut += translateResource(list[index], paramDefiner->resources()[index]);

		strIn = matches.suffix();
	}

	strOut += strIn;

	return strOut;
}

std::string UI_NetworkTransition::parseInterpretation(const std::string &format, const std::vector<ATN::ParameterMarshall*> &paramList, const std::vector<ATN::ResourceMarshall*> &resourceList, const ATN::IResourceHolder *paramDefiner) const
{
	return parseResources(parseParams(format, paramList, paramDefiner), resourceList, paramDefiner);
}

std::string UI_NetworkTransition::interpret()
{
	std::string str = "if ";

	if (ATN::Manager::hasInterpretation(m_transition->percept()->gameFunction()))
	{
		str += parseInterpretation(ATN::Manager::getInterpreration(m_transition->percept()->gameFunction()), m_transition->perceptParameterMarshalls(), m_transition->perceptResourceMarshalls(), (ATN::IResourceHolder*)m_transition->percept());
	}
	else
	{
		str += m_transition->percept()->name();
	}

	str += " then ";

	if (m_transition->effect() == nullptr)
	{
		// We omit "do nothing" since it adds no real information
	}
	else if (ATN::Manager::hasInterpretation(m_transition->effect()->gameFunction()))
	{
		str += parseInterpretation(ATN::Manager::getInterpreration(m_transition->effect()->gameFunction()), m_transition->effectParameterMarshalls(), m_transition->effectResourceMarshalls(), (ATN::IResourceHolder*)m_transition->effect());
	}
	else
	{
		str += m_transition->effect()->name();
	}

	return str;
}

void UI_NetworkTransition::layout()
{
	ui.label->setText(QString::fromStdString(interpret()));

	// Height is fixed so the connector fits the transition container
	int height = ui.label->height();

	ui.label->adjustSize();

	ui.label->setFixedHeight(height);

	m_connector->move(ui.label->x() + ui.label->width() + LABEL_PADDING, ui.label->y() + ui.label->height()/2 - m_connector->height()/2);

	adjustSize();
}
