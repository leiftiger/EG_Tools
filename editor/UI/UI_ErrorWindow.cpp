#include "UI_ErrorWindow.h"

UI_ErrorWindow::UI_ErrorWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

UI_ErrorWindow::~UI_ErrorWindow()
{
}

void UI_ErrorWindow::setErrorMessage(const std::string &str)
{
	ui.textErrorMessage->setPlainText(QString::fromStdString(str));
}