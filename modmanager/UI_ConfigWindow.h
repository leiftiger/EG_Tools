#pragma once

#include <QMainWindow>
#include "GeneratedFiles/ui_UI_ConfigWindow.h"

#include <QToolTip>

#include <fstream>
#include <string>
#include <vector>

class UI_ConfigWindow : public QMainWindow
{
	Q_OBJECT

public:
	UI_ConfigWindow(QWidget *parent = Q_NULLPTR);
	~UI_ConfigWindow();

	virtual void closeEvent(QCloseEvent *e) override;

private:
	Ui::UI_ConfigWindow ui;

	std::vector<std::string> m_configPaths;

	void loadConfig();
	void saveConfig();

public slots:
	void setPathEG(const QString &path);

	void openPathEG();

signals:
	void onClose();
};
