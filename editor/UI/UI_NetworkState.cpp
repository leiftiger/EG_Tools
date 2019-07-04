#include "UI_NetworkState.h"

UI_NetworkState::UI_NetworkState(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.connectOutNetwork->setConnectFlags(ConnectFlags::Above);
}

UI_NetworkState::~UI_NetworkState()
{

}
