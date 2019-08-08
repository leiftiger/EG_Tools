#pragma once

#include <QFrame>
#include <QMenu>

#include "ATN/ATN_Manager.h"

class UI_NetworkContainerStateFrame : public QFrame
{
	Q_OBJECT

protected:

	QPoint m_lastPos;

	bool m_readOnly = false;

public:
	UI_NetworkContainerStateFrame(QWidget *parent);
	~UI_NetworkContainerStateFrame();

	// Disallow state creation menu for this frame
	void setReadOnly(bool readonly);

protected slots:
	void openContextMenu(const QPoint &pos);

	void handleStateCreation();
	void handleStatePaste();

public:

signals:
	void requestStateCreation(const QPoint &pos);
	void requestStatePaste(const QPoint &pos);
};
