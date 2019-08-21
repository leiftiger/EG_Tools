#include "UI_ConfigWindow.h"

#include <QFileDialog>

UI_ConfigWindow::UI_ConfigWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	for (int i = 0; i < 1; i++)
		m_configPaths.push_back("");

	loadConfig();
}

UI_ConfigWindow::~UI_ConfigWindow()
{
	saveConfig();
}

void UI_ConfigWindow::closeEvent(QCloseEvent *e)
{
	saveConfig();
}

void UI_ConfigWindow::loadConfig()
{
	blockSignals(true);

	std::ifstream file("config.txt");

	if (!file.fail())
	{
		std::string line;

		for (int i = 0; i < m_configPaths.size(); i++)
		{
			std::getline(file, line);

			m_configPaths[i] = line;
		}
	}

	ui.textPathEG->setText(QString::fromStdString(m_configPaths[0]));

	blockSignals(false);
}

void UI_ConfigWindow::saveConfig()
{
	std::ofstream file("config.txt", std::ios::trunc);

	for (const std::string &path : m_configPaths)
		file << path << std::endl;
}

void UI_ConfigWindow::setPathEG(const QString &path)
{
	ui.textPathEG->setInputError(false);

	if (path.length() == 0)
		return;

	std::ifstream file(path.toStdString() + "/ReleaseExe/ELibBase-Release.dll");

	if (file.fail())
	{
		ui.textPathEG->setInputError(true);
		QToolTip::showText(this->mapToGlobal(ui.textPathEG->pos()), tr("Incorrect path"));
		return;
	}
	else
	{
		m_configPaths[0] = path.toStdString();
	}
}

void UI_ConfigWindow::openPathEG()
{
	QString qFile = QFileDialog::getExistingDirectory(this, tr("Select path to Evil Genius installation directory"), ui.textPathEG->text());

	ui.textPathEG->setText(qFile);
}