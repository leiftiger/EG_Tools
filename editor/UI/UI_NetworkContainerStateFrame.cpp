#include "UI_NetworkContainerStateFrame.h"

UI_NetworkContainerStateFrame::UI_NetworkContainerStateFrame(QWidget *parent)
	: QFrame(parent)
{
	this->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(openContextMenu(const QPoint&)));
}

UI_NetworkContainerStateFrame::~UI_NetworkContainerStateFrame()
{
}

void UI_NetworkContainerStateFrame::setReadOnly(bool readonly)
{
	m_readOnly = readonly;
}

void UI_NetworkContainerStateFrame::handleStateCreation()
{
	emit requestStateCreation(m_lastPos);
}

void UI_NetworkContainerStateFrame::handleStatePaste()
{
	emit requestStatePaste(m_lastPos);
}

void UI_NetworkContainerStateFrame::openContextMenu(const QPoint &pos)
{
	if (m_readOnly)
		return;

	m_lastPos = pos;

	QMenu contextMenu(tr("Context menu"), this);

	QAction actionCreate("Add new state here", this);
	QAction actionPaste("Paste state here", this);

	const ATN::Entry *curPaste = ATN::Manager::getStoredEntry();

	// Don't allow paste if there is none, or if it's the wrong type
	if (curPaste == nullptr || typeid(*curPaste) != typeid(ATN::State))
	{
		actionPaste.setEnabled(false);
	}

	connect(&actionCreate, SIGNAL(triggered()), this, SLOT(handleStateCreation()));
	connect(&actionPaste, SIGNAL(triggered()), this, SLOT(handleStatePaste()));

	connect(&contextMenu, SIGNAL(destroyed()), this, SLOT(update()));

	contextMenu.addAction(&actionCreate);
	contextMenu.addAction(&actionPaste);

	contextMenu.exec(mapToGlobal(pos));
}
