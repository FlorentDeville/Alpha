/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Tree/TreeWidget.h"

#include "Editors/Widgets/BaseModel.h"

#include "Widgets/Container.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	void CreateRecursiveWidgets(Widgets::Layout* pParentLayout, const BaseModel* pModel, int offsetX)
	{
		const int ITEM_HEIGHT = 15;

		//current item
		Widgets::Layout* pItemLayout = new Widgets::Layout();
		pItemLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
		pItemLayout->SetDirection(Widgets::Layout::Horizontal);
		pItemLayout->SetSize(DirectX::XMUINT2(0, ITEM_HEIGHT));
		pParentLayout->AddWidget(pItemLayout);

		//children layout
		offsetX = offsetX + 10;
		Widgets::Layout* pChildrenLayout = new Widgets::Layout();
		pChildrenLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
		pChildrenLayout->SetX(offsetX);
		pChildrenLayout->SetDirection(Widgets::Layout::Vertical);
		pParentLayout->AddWidget(pChildrenLayout);

		int count = pModel->GetSubRowCount(0);
		if (count > 0)
		{
			std::string expandIcon = Widgets::WidgetMgr::Get().GetEditorIconsPath() + "/expand.png";
			Widgets::Icon* pIcon = new Widgets::Icon(DirectX::XMINT2(0, 4), DirectX::XMUINT2(12, 12), expandIcon);
			pIcon->OnClick([pChildrenLayout](int, int) -> bool 
				{ 
					pChildrenLayout->IsEnabled() ? pChildrenLayout->Disable(false) : pChildrenLayout->Enable(false); 
					Widgets::WidgetMgr::Get().RequestResize();
					return true; 
				});
			pItemLayout->AddWidget(pIcon);
			
		}
		else
		{
			Widgets::Container* pEmptyContainer = new Widgets::Container(12, 12);
			pItemLayout->AddWidget(pEmptyContainer);
		}

		Widgets::Label* pItemLabel = new Widgets::Label(0, 0, 1, pModel->GetData(0));
		pItemLabel->SetSize(DirectX::XMUINT2(0, ITEM_HEIGHT));
		pItemLayout->AddWidget(pItemLabel);

		//recursively create widgets for children
		for (int rowId = 0; rowId < count; ++rowId)
		{
				
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
