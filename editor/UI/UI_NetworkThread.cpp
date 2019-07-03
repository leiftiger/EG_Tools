#include "UI_NetworkThread.h"

UI_NetworkThread::UI_NetworkThread(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

UI_NetworkThread::~UI_NetworkThread()
{
}


void UI_NetworkThread::setName(const QString &str)
{
	// Not fully initialized yet (will be called by initial value loaded)
	if (m_thread == nullptr)
		return;

	m_thread->setName(str.toStdString());
}