/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Tree/TreeWidget.h"

#include "Editors/Widgets/BaseModel.h"

#include "Rendering/Texture/TextureMgr.h"
#include "Rendering/Texture/Texture.h"

#include "Widgets/Container.h"
#include "Widgets/Button.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	TreeWidget::TreeWidget()
		: Widget()
		, m_pModel(nullptr)
		, m_isDirtyWidget(true)
		, m_onItemClicked()
		, m_pSelectedButton(nullptr)
		, m_pSelectedItemModel(nullptr)
	{
		SetSizeStyle(Widgets::Widget::STRETCH);

		{
			Rendering::Texture* pTexture = nullptr;
			Rendering::TextureMgr::Get().CreateTexture(&pTexture, m_collapsedIcon);
			std::string collapsedIconPath = Widgets::WidgetMgr::Get().GetEditorIconsPath() + "/collapsed.png";
			pTexture->Init(collapsedIconPath);
		}

		{
			Rendering::Texture* pTexture = nullptr;
			Rendering::TextureMgr::Get().CreateTexture(&pTexture, m_expandedIcon);
			std::string expandedIconPath = Widgets::WidgetMgr::Get().GetEditorIconsPath() + "/expanded.png";
			pTexture->Init(expandedIconPath);
		}
	}

	TreeWidget::~TreeWidget()
	{
		Rendering::TextureMgr::Get().DeleteTexture(m_collapsedIcon);
		Rendering::TextureMgr::Get().DeleteTexture(m_expandedIcon);
	}

	void TreeWidget::SetModel(BaseModel* pModel)
	{
		m_pModel = pModel;
		m_isDirtyWidget = true;
		m_pSelectedButton = nullptr;
		m_pSelectedItemModel = nullptr;
	}

	void TreeWidget::Update(uint64_t dt)
	{
		if (m_isDirtyWidget)
		{
			DeleteAllChildren();
			CreateWidgets();
			m_isDirtyWidget = false;
		}
	}

	void TreeWidget::OnItemClicked(const OnItemClickedCallback& callback)
	{
		m_onItemClicked = callback;
	}

	BaseModel* TreeWidget::GetSelectedItemModel()
	{
		return m_pSelectedItemModel;
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

	void TreeWidget::CreateRecursiveWidgets(Widgets::Layout* pParentLayout, BaseModel* pModel, int offsetX)
	{
		const int ITEM_HEIGHT = 20;

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
			Widgets::Icon* pIcon = new Widgets::Icon();
			pIcon->SetY(4);
			pIcon->SetSize(DirectX::XMUINT2(12, 12));
			pIcon->SetTextureId(m_expandedIcon);
			pIcon->OnClick([this, pChildrenLayout, pIcon]() -> bool
				{
					if (pChildrenLayout->IsEnabled())
					{
						pChildrenLayout->Disable(false);
						pIcon->SetTextureId(m_collapsedIcon);
					}
					else
					{
						pChildrenLayout->Enable(false);
						pIcon->SetTextureId(m_expandedIcon);
					}

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

		Widgets::Button* pButton = new Widgets::Button(0, ITEM_HEIGHT, 0, 0);
		pButton->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
		pButton->GetHoverStyle().ShowBorder(false);
		pButton->OnClick([this, pModel, pButton]() -> bool
			{
				if (m_pSelectedButton)
					m_pSelectedButton->Unselect();

				pButton->Select();
				m_pSelectedButton = pButton;
				m_pSelectedItemModel = pModel;

				if (m_onItemClicked)
					m_onItemClicked(pModel, 0);

				return true;
			});
		pItemLayout->AddWidget(pButton);

		Widgets::Label* pItemLabel = new Widgets::Label(0, 0, 1, pModel->GetData(0));
		pItemLabel->SetSize(DirectX::XMUINT2(0, ITEM_HEIGHT));
		pButton->AddWidget(pItemLabel);

		//recursively create widgets for children
		for (int rowId = 0; rowId < count; ++rowId)
		{

			CreateRecursiveWidgets(pChildrenLayout, pModel->GetSubModel(rowId, 0), offsetX);
		}
	}
}
