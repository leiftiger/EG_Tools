#include "UI_LineEdit.h"

UI_LineEdit::UI_LineEdit(QWidget *parent)
	: QLineEdit(parent)
{

}

UI_LineEdit::~UI_LineEdit()
{
}

bool UI_LineEdit::inputError() const
{
	return m_invalidInput;
}

void UI_LineEdit::setInputError(bool error)
{
	bool bDifferent = (m_invalidInput != error);

	m_invalidInput = error;

	if (bDifferent)
	{
		if (error)
		{
			m_oldStyleSheet = styleSheet();

			setStyleSheet(m_oldStyleSheet + "\nUI_LineEdit{ border: 1px solid rgba(255, 0, 0, 255); }");
		}
		else
		{
			setStyleSheet(m_oldStyleSheet);
		}
	}
}
