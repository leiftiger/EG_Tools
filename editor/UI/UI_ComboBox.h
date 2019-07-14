#pragma once

#include <QComboBox>
#include <QLineEdit>

// A combobox that can show that the current input is invalid
class UI_ComboBox : public QComboBox
{
	Q_OBJECT

public:
	UI_ComboBox(QWidget *parent = Q_NULLPTR);
	~UI_ComboBox();

	bool inputError() const;
	void setInputError(bool error);

	void setReadOnly(bool readonly);

	virtual void showPopup() override;

private:
	bool m_invalidInput = false;
	bool m_readOnly = false;

	QString m_oldStyleSheet = tr("");
};
