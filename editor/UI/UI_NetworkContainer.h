#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_NetworkContainer.h"

#include "ATN/ATN_Effect.h"
#include "ATN/ATN_Manager.h"
#include "ATN/ATN_Network.h"
#include "ATN/ATN_Percept.h"

class UI_NetworkContainer : public QWidget
{
	Q_OBJECT

private:

	ATN::Network *m_network;

	QStringList m_effectList;
	QStringList m_perceptList;

	// Initializes the two comboboxes used to select effect/percepts in the editor
	void initializeArgumentLists();

public:
	UI_NetworkContainer(QWidget *parent = Q_NULLPTR);
	~UI_NetworkContainer();

	void initializeFromID(std::int32_t id);

	// Gets the network this container represents
	const ATN::Network &network();

private:
	Ui::UI_NetworkContainer ui;
};
