/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Matrix/MatrixWidget.h"

#include "Widgets/Layout.h"
#include "Widgets/TextBox.h"

#include <sstream>

float StringToFloat(const std::string value)
{
	//first check if the string is a float
	std::istringstream iss(value);

	float f;
	iss >> f;
	if (!iss.eof() || iss.fail())
	{
		f = 0; // an error occured
	}

	return f;
}

namespace Editors
{
	MatrixWidget::MatrixWidget()
		: Widget()
		, m_cachedMatrix()
	{
		const int FIELD_HEIGHT = 20;
		const int FIELD_WIDTH = 50;

		SetSize(Core::UInt2(3 * FIELD_WIDTH, 4 * FIELD_HEIGHT));

		m_pLayout = new Widgets::Layout(3 * FIELD_WIDTH, 4 * FIELD_HEIGHT, 0, 0);
		m_pLayout->SetDirection(Widgets::Layout::Vertical);
		AddWidget(m_pLayout);

		for (int ii = 0; ii < 4; ++ii)
		{
			Widgets::Layout* pRowLayout = new Widgets::Layout(0, FIELD_HEIGHT, 0, 0);
			pRowLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH);
			pRowLayout->SetDirection(Widgets::Layout::Horizontal);
			m_pLayout->AddWidget(pRowLayout);

			for (int jj = 0; jj < 3; ++jj)
			{
				Widgets::TextBox* pNewTextBox = new Widgets::TextBox();
				pNewTextBox->SetSize(Core::UInt2(FIELD_WIDTH, FIELD_HEIGHT));
				pNewTextBox->OnValidate([this, ii, jj](const std::string& value) -> bool 
					{ 
						float floatValue = StringToFloat(value);
						m_cachedMatrix.Set(ii, jj, floatValue);
						if (m_onValueChanged)
							m_onValueChanged(m_cachedMatrix);

						return true;
					});
				m_pTextBox[ii][jj] = pNewTextBox;
				pRowLayout->AddWidget(pNewTextBox);
			}
		}
	}

	MatrixWidget::~MatrixWidget()
	{}

	void MatrixWidget::SetValue(const Core::Mat44f& value)
	{
		m_cachedMatrix = value;
		for (int ii = 0; ii < 4; ++ii)
		{
			for (int jj = 0; jj < 3; ++jj)
			{
				const int BUFFER_SIZE = 9;
				char buffer[BUFFER_SIZE] = { '\0' };
				snprintf(buffer, BUFFER_SIZE, "%.2f", value.Get(ii, jj));

				m_pTextBox[ii][jj]->SetText(buffer);
			}
		}
	}
}
