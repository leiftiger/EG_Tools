#pragma once

#include <QWidget>
#include <QScrollBar>

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

	ATN::Network *m_network = nullptr;

	QStringList m_effectList;
	QStringList m_perceptList;

	std::vector<UI_NetworkThread*> m_threads;

	// Initializes the two comboboxes used to select effect/percepts in the editor
	void initializeArgumentLists();

	void initializeThreads();

	// Orders a list of objects inside a listObject according to the list
	// (essentially a QListWidget, but containing dynamic objects)
	template <class T>
	void layoutSortables(std::vector<T*> &list, QWidget *listObject)
	{
		int x = 0, y = 0;

		for (size_t i = 0; i < list.size(); i++)
		{
			T *ut = list[i];

			// Disable first and last sort buttons
			ut->ui.buttonSortUp->setEnabled(i != 0);
			ut->ui.buttonSortDown->setEnabled(i != list.size() - 1);

			ut->move(0, y);

			y += ut->size().height();
		}

		listObject->setMinimumHeight(y);
	}

	// Moves an item up or down in the list, assuming it's possible
	template <class T>
	void itemMove(std::vector<T*> &list, T *item, bool up)
	{
		size_t originalIndex = -1;

		for (size_t i = 0; i < list.size(); i++)
		{
			T *o = list[i];

			if (o == item)
			{
				originalIndex = i;
				break;
			}
		}

		int dir = (!up ? 1 : -1);

		T *temp = list[originalIndex + dir];

		list[originalIndex + dir] = item;
		list[originalIndex] = temp;
	}

	// Removes an item from the list
	template <class T>
	void itemRemove(std::vector<T*> &list, T *item)
	{
		size_t originalIndex = -1;

		for (size_t i = 0; i < list.size(); i++)
		{
			T *o = list[i];

			if (o == item)
			{
				originalIndex = i;
				break;
			}
		}

		// Move all items after this index up one step
		for (size_t i = originalIndex; i < list.size()-1; i++)
		{
			list[i] = list[i + 1];
		}

		list.pop_back();
	}

	// Removes an ATN object from all lists and clears its used memory
	template <class T>
	void deleteATN(T *obj)
	{
		// Find list it belongs to
		ATN::List<ATN::Entry> *outList;

		ATN::Manager::findByID(obj->id(), outList);

		outList->remove(*(ATN::Entry*)obj);

		delete obj;
	}

	void initializeResources();
	void initializeVariables();

public:
	UI_NetworkContainer(QWidget *parent = Q_NULLPTR);
	~UI_NetworkContainer();

	void initializeFromID(std::int32_t id);

	// Gets the network this container represents
	const ATN::Network &network();

public slots:
	void threadCreate();
	void threadMoveUp();
	void threadMoveDown();
	void threadRemove();

	void stateCreate();
	void stateMoveUp();
	void stateMoveDown();
	void stateRemove();

	void resourceCreate();
	void resourceMoveUp();
	void resourceMoveDown();
	void resourceRemove();

	void variableCreate();
	void variableMoveUp();
	void variableMoveDown();
	void variableRemove();

	void setNetworkName(const QString &name);
	void setTransitionName(const QString &name);

	void setTransitionPercept(int index);
	void setTransitionEffect(int index);

	void deleteNetwork();
	void deleteTransition();

private:
	Ui::UI_NetworkContainer ui;
};
