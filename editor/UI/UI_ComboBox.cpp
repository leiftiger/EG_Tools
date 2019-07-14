#include "UI_ComboBox.h"

UI_ComboBox::UI_ComboBox(QWidget *parent)
	: QComboBox(parent)
{

}

UI_ComboBox::~UI_ComboBox()
{
}


bool UI_ComboBox::inputError() const
{
	return m_invalidInput;
}

void UI_ComboBox::setInputError(bool error)
{
	bool bDifferent = (m_invalidInput != error);

	m_invalidInput = error;

	if (bDifferent)
	{
		if (error)
		{
			m_oldStyleSheet = styleSheet();

			setStyleSheet(m_oldStyleSheet + "\nUI_ComboBox{ border: 1px solid rgba(255, 0, 0, 255); }");
		}
		else
		{
			setStyleSheet(m_oldStyleSheet);
		}
	}
}

void UI_ComboBox::setReadOnly(bool readonly)
{
	m_readOnly = readonly;

	lineEdit()->setReadOnly(readonly);
}

void UI_ComboBox::showPopup()
{
	if (!m_readOnly)
		QComboBox::showPopup();
	else
		QComboBox::hidePopup();
}
