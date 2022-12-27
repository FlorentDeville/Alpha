/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Matrix/MatrixWidget.h"

#include "Editors/Widgets/BaseModel.h"

#include "Widgets/Label.h"
#include "Widgets/Layout.h"

namespace Editors
{
	MatrixWidget::MatrixWidget()
		: Widget()
		, m_pModel(nullptr)
	{
		const int FIELD_HEIGHT = 15;
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
				Widgets::Label* pNewLabel = new Widgets::Label();
				pNewLabel->SetSize(DirectX::XMUINT2(FIELD_WIDTH, FIELD_HEIGHT));
				pNewLabel->SetSizeStyle(Widgets::Widget::DEFAULT);
				m_pLabel[ii][jj] = pNewLabel;
				pRowLayout->AddWidget(pNewLabel);
			}
		}
	}

	MatrixWidget::~MatrixWidget()
	{}

	void MatrixWidget::SetModel(const BaseModel* pModel)
	{
		m_pModel = pModel;
	}

	void MatrixWidget::Update()
	{
		for (int ii = 0; ii < 4; ++ii)
		{
			for (int jj = 0; jj < 3; ++jj)
			{
				m_pLabel[ii][jj]->SetText(m_pModel->GetData(ii, jj));
			}
		}
	}
}
