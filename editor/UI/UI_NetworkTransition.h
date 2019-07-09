#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_NetworkTransition.h"

#include <regex>
#include <string>

#include "UI_ConnectorStart.h"

#include "ATN/ATN_Network.h"
#include "ATN/ATN_Transition.h"

class UI_NetworkTransition : public QWidget
{
	Q_OBJECT

private:

	static const int LABEL_PADDING = 5;
	static const int STR_FORMAT_BUFF = 1024;

	ATN::Transition *m_transition;

	ATN::Network *m_network;

	std::string translateParameter(const ATN::ParameterMarshall *paramMarshall, const ATN::Parameter *parameter) const;
	std::string translateResource(const ATN::ResourceMarshall *resourceMarshall, const ATN::Resource *resource) const;

	std::string parseParams(const std::string &format, const std::vector<ATN::ParameterMarshall*> &paramList, const ATN::IResourceHolder *paramDefiner) const;
	std::string parseResources(const std::string &format, const std::vector<ATN::ResourceMarshall*> &resourceList, const ATN::IResourceHolder *paramDefiner) const;
	std::string parseInterpretation(const std::string &format, const std::vector<ATN::ParameterMarshall*> &paramList, const std::vector<ATN::ResourceMarshall*> &resourceList, const ATN::IResourceHolder *paramDefiner) const;

public:
	UI_NetworkTransition(QWidget *parent = Q_NULLPTR);
	~UI_NetworkTransition();

	void initialize(ATN::Transition *transition, ATN::Network *network);

	Ui::UI_NetworkTransition ui;

	UI_ConnectorStart *m_connector;

	// Returns an user-friendly string of what this transition does
	std::string interpret();

	void layout();
};
