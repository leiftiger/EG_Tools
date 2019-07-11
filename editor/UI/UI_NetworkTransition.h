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

	const QColor TRANSITION_COLOR_HOVERED = QColor(0, 0, 0, 32);
	const QColor TRANSITION_COLOR_HIGHLIGHTED = QColor(255, 255, 255, 255);

	const QColor TRANSITION_COLOR_BORDER = QColor(0, 0, 0, 255);
	static const int TRANSITION_SIZE_BORDER = 1;

	ATN::Transition *m_transition;

	ATN::Network *m_network;

	bool m_highlighted = false;

	std::string translateParameter(const ATN::ParameterMarshall *paramMarshall, const ATN::Parameter *parameter) const;
	std::string translateResource(const ATN::ResourceMarshall *resourceMarshall, const ATN::Resource *resource) const;

	std::string parseParams(const std::string &format, const std::vector<ATN::ParameterMarshall*> &paramList, const ATN::IResourceHolder *paramDefiner) const;
	std::string parseResources(const std::string &format, const std::vector<ATN::ResourceMarshall*> &resourceList, const ATN::IResourceHolder *paramDefiner) const;
	std::string parseInterpretation(const std::string &format, const std::vector<ATN::ParameterMarshall*> &paramList, const std::vector<ATN::ResourceMarshall*> &resourceList, const ATN::IResourceHolder *paramDefiner) const;

public:
	UI_NetworkTransition(QWidget *parent = Q_NULLPTR);
	~UI_NetworkTransition();

	virtual void paintEvent(QPaintEvent *e) override;
	virtual void mousePressEvent(QMouseEvent *e) override;

	void initialize(ATN::Transition *transition, ATN::Network *network);

	Ui::UI_NetworkTransition ui;

	UI_ConnectorStart *m_connector;

	// Stored as QObject to avoid circular class dependencies
	QObject *m_state;

	// Returns an user-friendly string of what this transition does
	std::string interpret();

	void layout();

	ATN::Transition *transition() const;

	void setHighlighted(bool highlighted);

signals:
	void unlockTransitionEditor();
};
