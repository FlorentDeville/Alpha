/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Matrix/MatrixWidget.h"

#include "Editors/Widgets/BaseModel.h"

//#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/TextBox.h"

namespace Editors
{
	MatrixWidget::MatrixWidget()
		: Widget()
		, m_pModel(nullptr)
		, m_isDirtyValue(false)
	{
		const int FIELD_HEIGHT = 20;
		const int FIELD_WIDTH = 50;

		SetSize(DirectX::XMUINT2(3 * FIELD_WIDTH, 4 * FIELD_HEIGHT));

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
				pNewTextBox->SetSize(DirectX::XMUINT2(FIELD_WIDTH, FIELD_HEIGHT));
				pNewTextBox->OnValidate([this, ii, jj](const std::string& value) -> bool { m_pModel->SetData(ii, jj, value); return true; });
				m_pTextBox[ii][jj] = pNewTextBox;
				pRowLayout->AddWidget(pNewTextBox);
			}
		}
	}

	MatrixWidget::~MatrixWidget()
	{}

	void MatrixWidget::SetModel(BaseModel* pModel)
	{
		m_pModel = pModel;
		m_isDirtyValue = true;
	}

	void MatrixWidget::Update(uint64_t dt)
	{
		if (m_isDirtyValue)
		{
			for (int ii = 0; ii < 4; ++ii)
			{
				for (int jj = 0; jj < 3; ++jj)
				{
					m_pTextBox[ii][jj]->SetText(m_pModel->GetData(ii, jj));
				}
			}
			m_isDirtyValue = false;
		}
	}
}
