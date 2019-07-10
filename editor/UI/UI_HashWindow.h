#pragma once

#include <QMainWindow>
#include "GeneratedFiles/ui_UI_HashWindow.h"

#include "utils.h"

class UI_HashWindow : public QMainWindow
{
	Q_OBJECT

public:
	UI_HashWindow(QWidget *parent = Q_NULLPTR);
	~UI_HashWindow();

public slots:

	void hashText(const QString &str);

private:
	Ui::UI_HashWindow ui;
};
