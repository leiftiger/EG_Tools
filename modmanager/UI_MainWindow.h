#pragma once

#include <QtWidgets/QMainWindow>

#include "GeneratedFiles/ui_UI_MainWindow.h"

#include "UI_AboutWindow.h"
#include "UI_ConfigWindow.h"

#include "UI_MergeWindow.h"

#include "ModPack.h"
#include "ModInfo.h"

#include <unordered_map>

#include "ResourceMerger.h"

class UI_MainWindow : public QMainWindow
{
	Q_OBJECT

private:

	UI_AboutWindow m_aboutWindow;

	std::unordered_map<std::string, ModInfo> m_modDetails;

	ModInfo *m_currentModDetails = nullptr;

	// Merge the mods in the list in the given order to the specified game directory
	void mergeMods(const std::string &basePath, const std::vector<std::string> &mods);

	// Adds base game DynamicResources files to the given resource merger
	void addBaseFiles(const std::string &basePath, ResourceMerger &merger) const;

	// Adds all files recursively from the given path to the mod pack
	void addModFiles(const std::string &path, ModPack &mod) const;

public:
	UI_MainWindow(QWidget *parent = Q_NULLPTR);

	// Directory in which all unaltered mods will be stored
	const char * const MOD_DISABLED_DIR = "/Mods";
	// Directory in which merged mods will be stored
	const char * const MOD_ENABLED_DIR = "/DynamicResources/ManagedMods";
	// File describing which mods are enabled and in what order they were compiled
	const char * const MOD_ENABLED_LIST = "/Mods/enabled.txt";
	// Directory in which backup files should be stored
	const char * const BACKUP_DIR = "/Backup";
	// Directory in which backed up DynamicResources files will be stored
	const char * const BACKUP_DIR_DYNAMIC = "/Backup/DynamicResources";

protected slots:
	void populateLists();

	void installMods();
	void uninstallMods();

	void openAboutWindow();
	void openConfigWindow();

	void saveModInfo();

	void viewModInfo(QListWidgetItem *item);

	void setModName(const QString &name);
	void setModAuthor(const QString &author);
	void setModDesc();

private:
	Ui::UI_MainWindowClass ui;
};
