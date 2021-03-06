#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_NetworkState.h"
#include <QStyle>

#include <vector>

#include "ATN/ATN_State.h"
#include "ATN/ATN_Network.h"
#include "ATN/ATN_Manager.h"

#include "UI_InputArgument.h"
#include "UI_InputResource.h"

class UI_NetworkState : public QWidget
{
	Q_OBJECT

private:

	std::vector<UI_InputResource*> m_resources;
	std::vector<UI_InputArgument*> m_arguments;

	// Geometries for restoring size
	std::vector<QRect> m_geometries;

public:
	UI_NetworkState(QWidget *parent = Q_NULLPTR);
	~UI_NetworkState();

	Ui::UI_NetworkState ui;

	ATN::State *m_state = nullptr;
	const ATN::Network *m_network = nullptr;

	// Handles scroll area resize events
	bool eventFilter(QObject *object, QEvent *event) override;

	// Initializes state from object
	void initialize(ATN::State *s, const ATN::Network *net);

	// Populates the network transition arguments from the marshalls
	void populateArguments();

	// Decreases size of state to a more minimized footprint
	void setMinimized(bool minimize);

	// Disables the state from being edited
	void setReadOnly(bool readonly);

protected slots:
	void openContextMenu(const QPoint &pos);

	void handleHighlight();
	void handleCopy();
	void handlePaste();
	void handlePasteLimited();

public slots:
	void enableExternalNetwork(bool enable);
	void selectExternalNetwork(int index);

	void setStateName(const QString &name);

	void viewExternalNetwork();

	void establishTransition();

signals:
	void openNetworkRequest(int id);
	void requestLayout();
	void requestNewTransition();

	void requestHighlight();
	void requestPaste();
	void requestPasteLimited();
};
