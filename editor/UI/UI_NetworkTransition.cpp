#include "UI_NetworkTransition.h"

UI_NetworkTransition::UI_NetworkTransition(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_connector = new UI_ConnectorStart(this);
	m_connector->setFixedSize(21, 21);

	// Ensure widget is redrawn on hover
	this->setAttribute(Qt::WA_Hover);

	setMouseTracking(true);
}

UI_NetworkTransition::~UI_NetworkTransition()
{
}

void UI_NetworkTransition::paintEvent(QPaintEvent *e)
{
	QStyleOption option = QStyleOption();
	option.initFrom(this);

	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	bool hovered = (option.state & QStyle::State_MouseOver) > 0;

	if (hovered || m_highlighted)
	{
		if (m_highlighted)
		{
			painter.setBrush(QBrush(TRANSITION_COLOR_HIGHLIGHTED, Qt::BrushStyle::SolidPattern));
		}
		else
		{
			painter.setBrush(QBrush(TRANSITION_COLOR_HOVERED, Qt::BrushStyle::SolidPattern));
		}

		painter.setPen(QPen(TRANSITION_COLOR_BORDER, TRANSITION_SIZE_BORDER));

		painter.drawRect(0, 0, width(), height());
	}

	painter.end();
}

void UI_NetworkTransition::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::MouseButton::LeftButton)
	{
		emit unlockTransitionEditor();
	}
}

void UI_NetworkTransition::initialize(ATN::Transition *transition, ATN::Network *network)
{
	m_transition = transition;
	m_network = network;
}

ATN::Transition * UI_NetworkTransition::transition() const
{
	return m_transition;
}

void UI_NetworkTransition::setHighlighted(bool highlighted)
{
	m_highlighted = highlighted;

	update();
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

	if (index == ATN::ResourceMarshall::INVALID_POINTER)
	{
		return "[MISSING RESOURCE]";
	}

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

		// This is a boolean-defined format string where we have definitions for true/false/unknown
		if (matches.prefix().length() == 0 && strIn.length() >= 2 && strIn.substr(0, 2) == "\t\t")
		{
			std::regex regStrBool("\t\t[^\t]+\t\t[^\t]+\t\t(.+)");

			if (strOut == "true")
			{
				regStrBool = std::regex("\t\t([^\t]+)\t\t");
			}
			else if (strOut == "false")
			{
				regStrBool = std::regex("\t\t[^\t]+\t\t([^\t]+)\t\t");
			}

			if (std::regex_search(strIn, matches, regStrBool))
			{
				strOut = "";
				strIn = matches[1];
			}
		}
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

	std::string perceptFunc = m_transition->percept()->gameFunction();
	std::string perceptFuncID = perceptFunc + std::string("::") + std::to_string(m_transition->percept()->id());

	if (ATN::Manager::hasInterpretation(perceptFunc))
	{
		str += parseInterpretation(ATN::Manager::getInterpreration(perceptFunc), m_transition->perceptParameterMarshalls(), m_transition->perceptResourceMarshalls(), (ATN::IResourceHolder*)m_transition->percept());
	}
	// The percepts' graph data may differ, so we format based on unique ID
	else if (ATN::Manager::hasInterpretation(perceptFuncID))
	{
		str += parseInterpretation(ATN::Manager::getInterpreration(perceptFuncID), m_transition->perceptParameterMarshalls(), m_transition->perceptResourceMarshalls(), (ATN::IResourceHolder*)m_transition->percept());
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
	else
	{
		std::string effectFunc = m_transition->effect()->gameFunction();
		std::string effectFuncID = effectFunc + std::string("::") + std::to_string(m_transition->effect()->id());

		if (ATN::Manager::hasInterpretation(effectFunc))
		{
			str += parseInterpretation(ATN::Manager::getInterpreration(effectFunc), m_transition->effectParameterMarshalls(), m_transition->effectResourceMarshalls(), (ATN::IResourceHolder*)m_transition->effect());
		}
		else if (ATN::Manager::hasInterpretation(effectFuncID))
		{
			str += parseInterpretation(ATN::Manager::getInterpreration(effectFuncID), m_transition->effectParameterMarshalls(), m_transition->effectResourceMarshalls(), (ATN::IResourceHolder*)m_transition->effect());
		}
		else
		{
			str += m_transition->effect()->name();
		}
	}

	return str;
}

void UI_NetworkTransition::layout()
{
	ui.label->setText(QString::fromStdString(m_transition->name()));

	// Height is fixed so the connector fits the transition container
	int height = ui.label->height();

	ui.label->adjustSize();

	ui.label->setFixedHeight(height);

	m_connector->move(ui.label->x() + ui.label->width() + LABEL_PADDING, ui.label->y() + ui.label->height()/2 - m_connector->height()/2);

	adjustSize();
}