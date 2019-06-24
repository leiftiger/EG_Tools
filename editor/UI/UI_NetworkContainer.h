#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_NetworkContainer.h"

#include "ATN/ATN_Effect.h"
#include "ATN/ATN_Manager.h"
#include "ATN/ATN_Network.h"
#include "ATN/ATN_Percept.h"
#include "ATN/ATN_Resources.h"
#include "ATN/ATN_State.h"
#include "ATN/ATN_Thread.h"
#include "ATN/ATN_Transition.h"

#include "UI_InputArgument.h"
#include "UI_InputResource.h"

#include "UI_NetworkResource.h"
#include "UI_NetworkState.h"
#include "UI_NetworkThread.h"
#include "UI_NetworkTransition.h"
#include "UI_NetworkVariable.h"

#include <vector>

class UI_NetworkContainer : public QWidget
{
	Q_OBJECT

private:

	ATN::Network *m_network;

	QStringList m_effectList;
	QStringList m_perceptList;

	std::vector<UI_NetworkThread*> m_threads;

	// Initializes the two comboboxes used to select effect/percepts in the editor
	void initializeArgumentLists();

	void initializeThreads();

	void initializeResources();
	void initializeVariables();

public:
	UI_NetworkContainer(QWidget *parent = Q_NULLPTR);
	~UI_NetworkContainer();

	void initializeFromID(std::int32_t id);

	// Gets the network this container represents
	const ATN::Network &network();

public slots:
	void threadMoveUp();
	void threadMoveDown();
	void threadRemove();

private:
	Ui::UI_NetworkContainer ui;
};
