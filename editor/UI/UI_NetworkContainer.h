#pragma once

#include <QWidget>
#include <QMessageBox>
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
#include "UI_NetworkParameter.h"

#include "UI_ConnectFlags.h"

#include "UI_Connection.h"
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
	QStringList m_parameterTypes = QStringList({
			"3D Gui Element", "Act Of Infamy ID", "Agent Group Type", "Agent Operation Type", "Animation Attachment", "Animation Biped Type",
			"Animation Flags", "Animation", "Boolean Value", "Camera View ID", "Character Tag", "Dialog Type", "Effect return value",
			"Entity Class", "Entity Type", "Event", "Floating Graphic", "Game Feature", "Game Flag", "GUI Control", "Hotspot",
			"Integer", "Interaction Reason", "LockName", "Mouse Button", "Move Flag", "Network ID", "Objective ID", "Region Activity",
			"Research card status", "Room Type", "ScaleType", "Sound Category ID", "Sound ID", "Spawn Type", "SpecialEffect", "String ID",
			"Terminate Interaction Priority", "UI State", "Video ID", "World Map Region", "World Region",
		});

	QStringList m_resourceTypes;

	std::vector<UI_NetworkThread*> m_threads;
	std::vector<UI_NetworkResource*> m_resources;
	std::vector<UI_NetworkParameter*> m_parameters;

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

	bool m_editingDisabled = false;

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

			if (!m_editingDisabled)
			{
				// Disable first and last sort buttons
				ut->ui.buttonSortUp->setEnabled(i != 0);
				ut->ui.buttonSortDown->setEnabled(i != list.size() - 1);
			}

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
	UI_NetworkParameter *createParameterUI(ATN::Parameter *parameter);
	UI_NetworkState *createStateUI(ATN::State *state);
	UI_NetworkTransition *createTransitionUI(ATN::Transition *transition, UI_NetworkState *uiStateFrom, UI_NetworkState *uiStateTo);


	// Initializes the two comboboxes used to select effect/percepts in the editor
	void initializeArgumentLists();

	void initializeThreads();

	void initializeResources();
	void initializeParameters();

	void layoutStates();

	void initializeStates();

	// Populates the resource and parameter arguments for the effects & percepts
	void populateTransitionArguments(std::vector<UI_InputArgument*> &argumentList, std::vector<UI_InputResource*> &resourceList, QWidget *argumentWidget, QWidget *resourceWidget, const ATN::IResourceHolder *resourceHolder, const std::vector<ATN::ParameterMarshall*> paramMarshalls, const std::vector<ATN::ResourceMarshall*> resourceMarshalls);

public:
	UI_NetworkContainer(QWidget *parent = Q_NULLPTR);
	~UI_NetworkContainer();

	void initializeFromID(std::int32_t id);

	// Gets the network this container represents
	const ATN::Network &network();

	// Sets the container to be read-only
	void setReadOnly(bool readonly);

public slots:
	void threadCreate();
	void threadMoveUp();
	void threadMoveDown();
	void threadRemove();

	void stateCreate();
	void stateMoveUp();
	void stateMoveDown();
	void stateRemove();

	void stateCreateAt(const QPoint &pos);
	void statePasteAt(const QPoint &pos);

	void resourceCreate();
	void resourceMoveUp();
	void resourceMoveDown();
	void resourceRemove();

	void resourceTypeChange(const QString &type);
	void resourceInternalChange(bool bInternal);

	void parameterCreate();
	void parameterMoveUp();
	void parameterMoveDown();
	void parameterRemove();

	void parameterTypeChange(const QString &type);

	void setNetworkName(const QString &name);
	void setTransitionName(const QString &name);

	void setTransitionPercept(int index);
	void setTransitionEffect(int index);

	void deleteNetwork();

	void findTransitions();

	void receiveOpenNetworkRequest(int id);

	void receiveStateLayoutRequest();

	void receiveStatePaste();
	void receiveStatePasteLimited();

	void receiveJumpRequest(QWidget *widget);

	void createNewConnector();

	void createNewTransition();

	void updateTransition();

	void editTransition();

	void updateTransitionInterpretation();

	void moveTransitionUp();

	void moveTransitionDown();

	void deleteTransition();

	void receiveTransitionPaste();

	// Repopulates the list of arguments in all states and the currently open transition
	// due to a change in parameter name or type
	void repopulateArguments();
	void repopulateArguments(bool bNeighborsToo);

	void maintainEditFramePositions();

	signals:
		void openNetworkRequest(int id);
		void closeNetworkRequest(int id);
		void findTransitionsRequest(int id);

		void repopulateNeighbors(int id);

private:
	Ui::UI_NetworkContainer ui;
};
