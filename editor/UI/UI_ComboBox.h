#pragma once

#include <QComboBox>

// A combobox that can show that the current input is invalid
class UI_ComboBox : public QComboBox
{
	Q_OBJECT

public:
	UI_ComboBox(QWidget *parent = Q_NULLPTR);
	~UI_ComboBox();

	bool inputError() const;
	void setInputError(bool error);

private:
	bool m_invalidInput = false;

	QString m_oldStyleSheet = tr("");
};
