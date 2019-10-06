#include "UI_MainWindow.h"
#include <QMessageBox>

#include <filesystem>
#include <fstream>
#include <unordered_set>

#include "utils.h"

#include "ModPack.h"
#include "ResourceMerger.h"
#include "ResourcePacks.h"

#include <sstream>

void UI_MainWindow::mergeMods(const std::string &basePath, const std::vector<std::string> &mods)
{
	std::filesystem::create_directory(basePath + MOD_ENABLED_DIR);

	std::vector<std::string> strPacks = util::configPaths("files/resource_packs.txt");

	for (std::string &str : strPacks)
	{
		str = basePath + str;
	}

	// TODO: Read Current.opt to retrieve game locale to allow for non-english patching

	ResourceMerger *merger = new ResourceMerger(new ResourcePacks(strPacks), basePath + MOD_ENABLED_DIR);

	for (const std::string &mod : mods)
	{
		ModPack *modPack = new ModPack(mod);

		addModFiles(basePath + MOD_DISABLED_DIR + "/" + mod, *modPack);

		merger->addMod(modPack);
	}

	UI_MergeWindow *uiResult = new UI_MergeWindow(this, merger);

	uiResult->show();
}

void UI_MainWindow::addModFiles(const std::string &path, ModPack &mod)
{
	for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			addModFiles(entry.path().string(), mod);
		}
		else
		{
			std::string filename = entry.path().filename().string();

			// Skip mod info files as they're just dummies to contain info for non-managed mods
			if (filename.length() < strlen("modinfo_") || filename.substr(0, strlen("modinfo_")) != "modinfo_")
				mod.addFile(path + "/" + filename);
		}
	}
}

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

		if (!entry.is_directory())
			continue;

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
				std::string modName = path.filename().string();

				// This is the only mod we can be relatively certain of
				if (modName == "Patch")
					modName = "Unofficial Patch";

				std::filesystem::remove_all(modPath + "/" + modName);
				std::filesystem::rename(path, modPath + "/" + modName);
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

	std::unordered_set<std::string> isEnabled;

	for (std::string &mod : enabledMods)
	{
		if (std::filesystem::exists(modPath + "/" + mod))
		{
			ui.listWidgetEnabled->addItem(QString::fromStdString(mod));

			isEnabled.emplace(mod);
		}
	}

	QStringList disabledMods;

	for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(modPath))
	{
		if (entry.is_directory())
		{
			std::string mod = entry.path().filename().string();

			if (isEnabled.find(mod) == isEnabled.end())
				disabledMods.push_back(QString::fromStdString(mod));

			bool hasModInfo = false;

			for (const std::filesystem::directory_entry &modEntry : std::filesystem::directory_iterator(entry.path()))
			{
				std::string &filename = modEntry.path().filename().string();

				if (filename.length() >= strlen("modinfo_") && filename.substr(0, strlen("modinfo_")) == "modinfo_")
				{
					ModInfo info;

					info.setFilename(modEntry.path().string());

					std::ifstream fs(info.filename());

					fs >> info;

					m_modDetails[mod] = info;

					hasModInfo = true;
				}
			}

			if (!hasModInfo)
			{
				ModInfo info;

				std::string modInfoName("modinfo_");

				for (char c : mod)
				{
					if (c != ' ')
						modInfoName += tolower(c);
					else
						modInfoName += '_';
				}

				info.setFilename(modPath + "/" + mod + "/" + modInfoName + ".xml");

				info["name"] = mod;
				info["author"] = "N/A";
				info["desc"] = "";

				if (mod == "Custom")
				{
					info["name"] = "Poorly Named Mod";
					info["desc"] = "This is a poorly named mod, please tell the mod author that they don't have to name their DynamicResources folder \"Custom\" for it to be recognized by the game.<br><br>You may rename the folder named \"Custom\" in \"Evil Genius" + std::string(MOD_DISABLED_DIR) + "\" to something more appropriate if you know what mod it is.";
				}

				m_modDetails[mod] = info;
			}
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

	uninstallMods();

	std::ofstream file(config[0] + MOD_ENABLED_LIST, std::ios::trunc);

	std::vector<std::string> enabledMods;

	for (int i = 0; i < ui.listWidgetEnabled->count(); i++)
	{
		enabledMods.push_back(ui.listWidgetEnabled->item(i)->text().toStdString());
	}

	for(const std::string &mod : enabledMods)
	{
		file << mod << std::endl;
	}

	mergeMods(config[0], enabledMods);
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

void UI_MainWindow::saveModInfo()
{
	ModInfo &info = *m_currentModDetails;

	std::ofstream fs(info.filename(), std::ios::trunc);

	fs << info;
}

void UI_MainWindow::viewModInfo(QListWidgetItem *item)
{
	m_currentModDetails = nullptr;

	std::string mod = item->text().toStdString();

	ModInfo &info = m_modDetails[mod];

	ui.modFrame->setEnabled(true);

	ui.textModName->setText(QString::fromStdString(info["name"]));
	ui.textModAuthor->setText(QString::fromStdString(info["author"]));
	ui.textModDesc->setHtml(QString::fromStdString(info["desc"]));

	m_currentModDetails = &info;
}

void UI_MainWindow::setModName(const QString &name)
{
	if (m_currentModDetails == nullptr)
		return;

	(*m_currentModDetails)["name"] = name.toStdString();
}

void UI_MainWindow::setModAuthor(const QString &author)
{
	if (m_currentModDetails == nullptr)
		return;

	(*m_currentModDetails)["author"] = author.toStdString();
}

void UI_MainWindow::setModDesc()
{
	if (m_currentModDetails == nullptr)
		return;

	(*m_currentModDetails)["desc"] = ui.textModDesc->toPlainText().toStdString();
}
