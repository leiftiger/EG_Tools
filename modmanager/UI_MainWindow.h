#pragma once

#include <QtWidgets/QMainWindow>

#include "GeneratedFiles/ui_UI_MainWindow.h"

#include "UI_AboutWindow.h"
#include "UI_ConfigWindow.h"

#include "UI_MergeWindow.h"

#include "ModPack.h"

class UI_MainWindow : public QMainWindow
{
	Q_OBJECT

private:

	UI_AboutWindow m_aboutWindow;

	// Merge the mods in the list in the given order to the specified game directory
	void mergeMods(const std::string &basePath, const std::vector<std::string> &mods);

	// Adds all files recursively from the given path to the mod pack
	void addModFiles(const std::string &path, ModPack &mod);

public:
	UI_MainWindow(QWidget *parent = Q_NULLPTR);

	// Directory in which all unaltered mods will be stored
	const char * const MOD_DISABLED_DIR = "/Mods";
	// Directory in which merged mods will be stored
	const char * const MOD_ENABLED_DIR = "/DynamicResources/ManagedMods";
	// File describing which mods are enabled and in what order they were compiled
	const char * const MOD_ENABLED_LIST = "/Mods/enabled.txt";

protected slots:
	void populateLists();

	void installMods();
	void uninstallMods();

	void saveModInfo();

	void openAboutWindow();
	void openConfigWindow();

private:
	Ui::UI_MainWindowClass ui;
};
