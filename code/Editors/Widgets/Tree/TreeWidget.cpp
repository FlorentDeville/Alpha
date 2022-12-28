/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Tree/TreeWidget.h"

#include "Editors/Widgets/BaseModel.h"

#include "Widgets/Container.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	void CreateRecursiveWidgets(Widgets::Layout* pParentLayout, const BaseModel* pModel, int offsetX)
	{
		//label
		Widgets::Label* pItemLabel = new Widgets::Label(0, 0, 1, pModel->GetData(0));
		pItemLabel->SetSize(DirectX::XMUINT2(0, 15));
		pParentLayout->AddWidget(pItemLabel);

		offsetX = offsetX + 10;
		Widgets::Layout* pChildrenLayout = new Widgets::Layout();
		pChildrenLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
		pChildrenLayout->SetX(offsetX);
		pChildrenLayout->SetDirection(Widgets::Layout::Vertical);
		pParentLayout->AddWidget(pChildrenLayout);

		int count = pModel->GetSubRowCount(0);
		for (int rowId = 0; rowId < count; ++rowId)
		{
			//recursively create widgets for children	
			CreateRecursiveWidgets(pChildrenLayout, pModel->GetSubModel(rowId), offsetX);
		}
	}

	TreeWidget::TreeWidget()
		: Widget()
		, m_pModel(nullptr)
		, m_isDirtyWidget(true)
	{
		SetSizeStyle(Widgets::Widget::STRETCH);
	}

	TreeWidget::~TreeWidget()
	{}

	void TreeWidget::SetModel(const BaseModel* pModel)
	{
		m_pModel = pModel;
	}

	void TreeWidget::Update()
	{
		if (m_isDirtyWidget)
		{
			CreateWidgets();
			m_isDirtyWidget = false;
		}
	}

	void TreeWidget::CreateWidgets()
	{
		// Every item is a layout. The layout contains a label for the name of the node and another layout
		// for its children.
		
		Widgets::Layout* pRootLayout = new Widgets::Layout();
		pRootLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pRootLayout->SetDirection(Widgets::Layout::Vertical);
		AddWidget(pRootLayout);

		CreateRecursiveWidgets(pRootLayout, m_pModel, 0);

		Widgets::WidgetMgr::Get().RequestResize();
	}
}
