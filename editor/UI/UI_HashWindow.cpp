#include "UI_HashWindow.h"

UI_HashWindow::UI_HashWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

UI_HashWindow::~UI_HashWindow()
{
}

void UI_HashWindow::hashText(const QString &str)
{
	std::int32_t hash = util::hashElixir(str.toStdString());

	ui.textHashS->setText(QString::fromStdString(std::to_string(hash)));
	ui.textHashU->setText(QString::fromStdString(std::to_string((std::uint32_t)hash)));
}