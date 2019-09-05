#include "UI_MainWindow.h"
#include <QMessageBox>

#include <filesystem>
#include <fstream>
#include <unordered_map>

#include "utils.h"

UI_MainWindow::UI_MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	populateLists();
}

void UI_MainWindow::populateLists()
{
	ui.listWidgetEnabled->clear();
	ui.listWidgetDisabled->clear();

	std::vector<std::string> config = util::configPaths("config.txt");

	if (config.size() == 0)
	{
		openConfigWindow();
		return;
	}

	std::string egPath = config[0];

	if (egPath.length() == 0)
	{
		openConfigWindow();
		return;
	}

	std::string modPath = egPath + MOD_DISABLED_DIR;

	std::filesystem::create_directory(modPath);

	std::vector<std::filesystem::path> unmanagedMods;

	for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(egPath + "/DynamicResources"))
	{
		std::string subDir = entry.path().filename().string();

		// Assuming that each mod uses their own directory, filter out from the default folders
		if (subDir != "BalancingSpreadsheets" &&
			subDir != "CodeGeneratedData" &&
			subDir != "Config" &&
			subDir != "Layouts" &&
			subDir != "SecretLayouts" &&
			subDir != "Video" &&
			subDir != "ResourceList.erl" &&
			subDir != "ManagedMods")
		{
			unmanagedMods.push_back(entry.path());
		}
	}

	if (unmanagedMods.size() > 0)
	{
		QMessageBox msg;

		msg.setWindowFlags(Qt::Dialog | Qt::Desktop | Qt::WindowStaysOnTopHint);
		msg.setIcon(QMessageBox::Icon::Question);

		msg.setWindowTitle(tr(" "));
		msg.setText(tr("<span style=\"font-size:12pt;\"><b>Unmanaged mods</b></span>"));

		QString strMods;

		for (std::filesystem::path &path : unmanagedMods)
		{
			strMods.append("\t");
			strMods.append(QString::fromStdString(path.filename().string()));
			strMods.append("\n");
		}

		msg.setInformativeText(tr("There are some mods installed that aren't managed by the Mod Manager, do you want to make them managed now?\n\n") + strMods + "\n\n" +tr("Not doing this could potentially cause conflicts that the Mod Manager is designed to counteract."));

		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setDefaultButton(QMessageBox::Yes);

		QApplication::beep();
		int msgRet = msg.exec();

		switch (msgRet)
		{
		case QMessageBox::Yes:
		{
			for (std::filesystem::path &path : unmanagedMods)
			{
				std::filesystem::remove_all(modPath + "/" + path.filename().string());
				std::filesystem::rename(path, modPath + "/" + path.filename().string());
			}
			break;
		}
		case QMessageBox::No:
		{
			break;
		}
		}
	}

	std::vector<std::string> enabledMods = util::configPaths(egPath + MOD_ENABLED_LIST);

	std::unordered_map<std::string, bool> isEnabled;

	for (std::string &mod : enabledMods)
	{
		ui.listWidgetEnabled->addItem(QString::fromStdString(mod));

		isEnabled[mod] = true;
	}

	QStringList disabledMods;

	for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(modPath))
	{
		if (entry.is_directory())
		{
			std::string mod = entry.path().filename().string();

			if (isEnabled.find(mod) == isEnabled.end())
				disabledMods.push_back(QString::fromStdString(mod));
		}
	}

	ui.listWidgetDisabled->addItems(disabledMods);
}

void UI_MainWindow::installMods()
{
	std::vector<std::string> config = util::configPaths("config.txt");

	if (config.size() == 0 || config[0].length() == 0)
	{
		QMessageBox msg;

		msg.setWindowFlags(Qt::Dialog | Qt::Desktop);
		msg.setIcon(QMessageBox::Icon::Critical);

		msg.setWindowTitle(tr(" "));
		msg.setText(tr("<span style=\"font-size:12pt;\"><b>Evil Genius installation path not set!</b></span>"));

		msg.setInformativeText(tr("You have not set a path to your Evil Genius installation, the requested operation requires it to be set."));

		msg.setStandardButtons(QMessageBox::Ok);
		msg.setDefaultButton(QMessageBox::Ok);

		QApplication::beep();
		int msgRet = msg.exec();

		return;
	}

	std::ofstream file(config[0] + MOD_ENABLED_LIST, std::ios::trunc);

	for (int i = 0; i < ui.listWidgetEnabled->count(); i++)
	{
		file << ui.listWidgetEnabled->item(i)->text().toStdString() << std::endl;
	}

	// TODO: Do mod content merging
}

void UI_MainWindow::uninstallMods()
{
	std::vector<std::string> config = util::configPaths("config.txt");

	if (config.size() == 0 || config[0].length() == 0)
	{
		QMessageBox msg;

		msg.setWindowFlags(Qt::Dialog | Qt::Desktop);
		msg.setIcon(QMessageBox::Icon::Critical);

		msg.setWindowTitle(tr(" "));
		msg.setText(tr("<span style=\"font-size:12pt;\"><b>Evil Genius installation path not set!</b></span>"));

		msg.setInformativeText(tr("You have not set a path to your Evil Genius installation, the requested operation requires it to be set."));

		msg.setStandardButtons(QMessageBox::Ok);
		msg.setDefaultButton(QMessageBox::Ok);

		QApplication::beep();
		int msgRet = msg.exec();
		return;
	}

	std::filesystem::remove_all(config[0] + MOD_ENABLED_DIR);
}

void UI_MainWindow::saveModInfo()
{

}

void UI_MainWindow::openAboutWindow()
{
	m_aboutWindow.show();
}

void UI_MainWindow::openConfigWindow()
{
	UI_ConfigWindow *window = new UI_ConfigWindow(this);
	window->show();

	connect(window, SIGNAL(onClose()), this, SLOT(populateLists()));
}
