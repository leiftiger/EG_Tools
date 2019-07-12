#pragma once

#include <QLineEdit>

// A line edit that can show that the current text is invalid
class UI_LineEdit : public QLineEdit
{
	Q_OBJECT

public:
	UI_LineEdit(QWidget *parent = Q_NULLPTR);
	~UI_LineEdit();

	bool inputError() const;
	void setInputError(bool error);

private:
	bool m_invalidInput = false;

	QString m_oldStyleSheet = tr("");
};
