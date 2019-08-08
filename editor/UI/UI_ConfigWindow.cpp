#include "UI_ConfigWindow.h"

#include <QFileDialog>

UI_ConfigWindow::UI_ConfigWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	for (int i = 0; i < 3; i++)
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

	ui.textPathResource->setText(QString::fromStdString(m_configPaths[0]));
	ui.textPathText->setText(QString::fromStdString(m_configPaths[1]));
	ui.textPathDynamic->setText(QString::fromStdString(m_configPaths[2]));

	blockSignals(false);
}

void UI_ConfigWindow::saveConfig()
{
	std::ofstream file("config.txt", std::ios::trunc);

	for (const std::string &path : m_configPaths)
		file << path << std::endl;
}

void UI_ConfigWindow::setPathResource(const QString &path)
{
	ui.textPathResource->setInputError(false);

	if (path.length() == 0)
		return;

	std::ifstream file(path.toStdString() + "/ResourceList.erl");

	if (file.fail())
	{
		ui.textPathResource->setInputError(true);
		QToolTip::showText(ui.textPathResource->mapToGlobal(ui.textPathResource->pos()), tr("Incorrect path"));
		return;
	}
	else
	{
		m_configPaths[0] = ui.textPathResource->text().toStdString();
	}
}

void UI_ConfigWindow::setPathText(const QString &path)
{
	ui.textPathText->setInputError(false);

	if (path.length() == 0)
		return;

	std::ifstream file(path.toStdString() + "/ResourceList.erl");

	if (file.fail())
	{
		ui.textPathText->setInputError(true);
		QToolTip::showText(this->mapToGlobal(ui.textPathText->pos()), tr("Incorrect path"));
		return;
	}
	else
	{
		m_configPaths[1] = ui.textPathText->text().toStdString();
	}
}

void UI_ConfigWindow::setPathDynamic(const QString &path)
{
	ui.textPathDynamic->setInputError(false);

	if (path.length() == 0)
		return;

	std::ifstream file(path.toStdString() + "/ResourceList.erl");

	if (file.fail())
	{
		ui.textPathDynamic->setInputError(true);
		QToolTip::showText(this->mapToGlobal(ui.textPathDynamic->pos()), tr("Incorrect path"));
		return;
	}
	else
	{
		m_configPaths[2] = ui.textPathDynamic->text().toStdString();
	}

	if (!path.endsWith("DynamicResources", Qt::CaseInsensitive))
	{
		ui.textPathDynamic->setInputError(true);
		QToolTip::showText(this->mapToGlobal(ui.textPathDynamic->pos()), tr("Wrong path"));
		return;
	}
}

void UI_ConfigWindow::openPathResource()
{
	QString qFile = QFileDialog::getExistingDirectory(this, tr("Select path to extracted Resource.erb"), ui.textPathResource->text());

	ui.textPathResource->setText(qFile);
}

void UI_ConfigWindow::openPathText()
{
	QString qFile = QFileDialog::getExistingDirectory(this, tr("Select path to extracted LResource_en_UK.erb"), ui.textPathText->text());

	ui.textPathText->setText(qFile);
}

void UI_ConfigWindow::openPathDynamic()
{
	QString qFile = QFileDialog::getExistingDirectory(this, tr("Select path to Evil Genius/DynamicResources"), ui.textPathDynamic->text());

	ui.textPathDynamic->setText(qFile);
}

void UI_ConfigWindow::setPathERB(const QString &path)
{
	ui.textPathERB->setInputError(false);

	if (path.length() == 0)
		return;

	std::ifstream file(path.toStdString());

	if (file.fail())
	{
		ui.textPathERB->setInputError(true);
		QToolTip::showText(this->mapToGlobal(ui.textPathERB->pos()), tr("Cannot find file"));
		return;
	}
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
}

void UI_ConfigWindow::openPathERB()
{
	QString qFile = QFileDialog::getOpenFileName(this, tr(""), ui.textPathERB->text(), tr("Executable files (*.exe)"));

	ui.textPathERB->setText(qFile);
}

void UI_ConfigWindow::openPathEG()
{
	QString qFile = QFileDialog::getExistingDirectory(this, tr("Select path to Evil Genius installation directory"), ui.textPathEG->text());

	ui.textPathEG->setText(qFile);
}