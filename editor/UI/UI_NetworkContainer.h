#pragma once

#include <QWidget>
#include <QScrollBar>
#include <QToolTip>

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

#include "UI_ConnectFlags.h"

#include "UI_Connector.h"
#include "UI_ConnectorStart.h"
#include "UI_ConnectorEnd.h"

#include "NetworkContainerProxy.h"

#include <vector>
#include <unordered_map>

class UI_NetworkContainer : public QWidget
{
	Q_OBJECT

private:

	ATN::Network *m_network = nullptr;

	QStringList m_effectList;
	QStringList m_perceptList;

	// Hardcoded since there are no other types
	QStringList m_variableTypes = QStringList({
			tr("3D Gui Element"), tr("Act Of Infamy ID"), tr("Agent Group Type"), tr("Agent Operation Type"), tr("Animation Attachment"), tr("Animation Biped Type"),
			tr("Animation Flags"), tr("Animation"), tr("Boolean Value"), tr("Camera View ID"), tr("Character Tag"), tr("Dialog Type"), tr("Effect return value"),
			tr("Entity Class"), tr("Entity Type"), tr("Event"), tr("Floating Graphic"), tr("Game Feature"), tr("Game Flag"), tr("GUI Control"), tr("Hotspot"),
			tr("Integer"), tr("Interaction Reason"), tr("LockName"), tr("Mouse Button"), tr("Move Flag"), tr("Network ID"), tr("Objective ID"), tr("Region Activity"),
			tr("Research card status"), tr("Room Type"), tr("ScaleType"), tr("Sound Category ID"), tr("Sound ID"), tr("Spawn Type"), tr("SpecialEffect"), tr("String ID"),
			tr("Terminate Interaction Priority"), tr("UI State"), tr("Video ID"), tr("World Map Region"), tr("World Region"),
		});

	QStringList m_resourceTypes;

	std::vector<UI_NetworkThread*> m_threads;
	std::vector<UI_NetworkResource*> m_resources;
	std::vector<UI_NetworkVariable*> m_variables;

	std::vector<UI_NetworkState*> m_states;

	QPoint m_initialFrameNetworkPos;
	QPoint m_initialFrameTransitionPos;

	// Total size of network container
	QSize m_networkContentsMinimumSize;

	NetworkContainerProxy m_proxy;

	UI_NetworkState *m_currentEditState = nullptr;
	UI_NetworkTransition *m_currentEditTransition = nullptr;

	std::vector<UI_InputArgument*> m_currentTransitionEffectArguments;
	std::vector<UI_InputResource*> m_currentTransitionEffectResources;

	std::vector<UI_InputArgument*> m_currentTransitionPerceptArguments;
	std::vector<UI_InputResource*> m_currentTransitionPerceptResources;

	// Minimum distance between two states
	const int STATE_MARGIN = 100;

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

	UI_NetworkThread *createThreadUI(ATN::Thread *thread);
	UI_NetworkResource *createResourceUI(ATN::Resource *resource);
	UI_NetworkVariable *createVariableUI(ATN::Parameter *parameter);
	UI_NetworkState *createStateUI(ATN::State *state);
	UI_NetworkTransition *createTransitionUI(ATN::Transition *transition, UI_NetworkState *uiStateFrom, UI_NetworkState *uiStateTo);


	// Initializes the two comboboxes used to select effect/percepts in the editor
	void initializeArgumentLists();

	void initializeThreads();

	void initializeResources();
	void initializeVariables();

	void layoutStates();

	void initializeStates();

	// Populates the resource and parameter arguments for the effects & percepts
	void populateTransitionArguments(std::vector<UI_InputArgument*> &argumentList, std::vector<UI_InputResource*> &resourceList, QWidget *argumentWidget, QWidget *resourceWidget, const ATN::IResourceHolder *resourceHolder, const std::vector<ATN::ParameterMarshall*> paramMarshalls, const std::vector<ATN::ResourceMarshall*> resourceMarshalls);

	// Clears all references to the variable at specified index unless it is still capable of being cast to the specified resource
	// Similar to ATN::Network::removeResourceMarshall(), but deals with changed type rather than removing anything
	void clearResourceReferences(std::int64_t index, const ATN::Resource &resource);

	// Clears all references to the variable at specified index
	// Similar to ATN::Network::removeParameterMarshall(), but deals with changed type rather than removing anything
	void clearVariableReferences(std::int64_t index);

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

	void resourceTypeChange(const QString &type);
	void resourceInternalChange(bool bInternal);

	void variableCreate();
	void variableMoveUp();
	void variableMoveDown();
	void variableRemove();

	void variableTypeChange(const QString &type);

	void setNetworkName(const QString &name);
	void setTransitionName(const QString &name);

	void setTransitionPercept(int index);
	void setTransitionEffect(int index);

	void deleteNetwork();

	void findTransitions();

	void receiveOpenNetworkRequest(int id);

	void receiveStateLayoutRequest();

	void createNewConnector();

	void createNewTransition();

	void updateTransition();

	void editTransition();

	void moveTransitionUp();

	void moveTransitionDown();

	void deleteTransition();

	// Repopulates the list of arguments in all states and the currently open transition
	// due to a change in variable name or type
	void repopulateArguments();
	void repopulateArguments(bool bNeighborsToo);

	void maintainEditFramePositions();

	signals:
		void openNetworkRequest(int id);
		void findTransitionsRequest(int id);

		void repopulateNeighbors(int id);

private:
	Ui::UI_NetworkContainer ui;
};
