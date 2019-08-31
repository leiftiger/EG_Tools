#include "UI_NetworkThread.h"

UI_NetworkThread::UI_NetworkThread(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.connector->setConnectFlags((ConnectFlags)(ConnectFlags::Above | ConnectFlags::OffsetLeft));

	connect(ui.connector, SIGNAL(establishTransition()), this, SLOT(establishTransition()));
}

UI_NetworkThread::~UI_NetworkThread()
{
}

void UI_NetworkThread::establishTransition()
{
	UI_NetworkState *state = (UI_NetworkState*)ui.connector->connection()->end()->parent();

	m_thread->setState(state->m_state);
}


void UI_NetworkThread::setName(const QString &name)
{
	// Not fully initialized yet (will be called by initial value loaded)
	if (m_thread == nullptr)
		return;

	ATN::List<ATN::Entry> *netList;

	ATN::Manager::findByID(m_thread->id(), netList);

	netList->updateName(*m_thread, name.toStdString());
}