/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/AssetId/AssetIdWidget.h"

#include "Editors/Widgets/AssetId/AssetIdModel.h"
#include "Editors/Widgets/List/ListWidget.h"
#include "Editors/Widgets/List/Models/AssetListModel.h"

#include "Systems/Assets/AssetId.h"

#include "Widgets/Button.h"
#include "Widgets/Container.h"
#include "Widgets/Label.h"
#include "Widgets/ModalWindow.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	AssetIdWidget::AssetIdWidget()
		: Layout()
		, m_pModel(nullptr)
	{
		SetDirection(Widgets::Layout::Horizontal_Reverse);

		m_pButton = new Widgets::Button(20, 20, 0, 0);
		m_pButton->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
		AddWidget(m_pButton);

		Widgets::Label* pButtonLabel = new Widgets::Label(0, 0, 1, "...");
		pButtonLabel->SetSizeStyle(Widgets::Widget::STRETCH);
		pButtonLabel->SetX(6);
		pButtonLabel->SetY(-3);
		m_pButton->AddWidget(pButtonLabel);
		m_pButton->OnClick([this](int, int)->bool
			{
				Widgets::ModalWindow* pWindow = new Widgets::ModalWindow();
				pWindow->SetSize(DirectX::XMUINT2(500, 500));
				pWindow->SetSizeStyle(Widgets::Widget::DEFAULT);
				pWindow->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

				//vlayout
				Widgets::Layout* pVLayout = new Widgets::Layout();
				pVLayout->SetDirection(Widgets::Layout::Vertical);
				pVLayout->SetSizeStyle(Widgets::Widget::STRETCH);
				pWindow->AddWidget(pVLayout);

				//list
				Editors::ListWidget* pList = new Editors::ListWidget();
				pList->SetSize(DirectX::XMUINT2(500, 460));
				pList->SetColumnSize(0, 75);

				PropertyType type = m_pModel->GetDataType(0, 0);
				Systems::AssetType assetType = Systems::kMesh;
				if (type == PropertyType::kAssetMaterial)
					assetType = Systems::kMaterial;
				Editors::AssetListModel* pModel = new Editors::AssetListModel(assetType);
				pList->SetModel(pModel);
				pVLayout->AddWidget(pList);

				//button ok escape
				Widgets::Layout* pHLayout = new Widgets::Layout();
				pHLayout->SetDirection(Widgets::Layout::Horizontal);
				pHLayout->SetSizeStyle(Widgets::Widget::STRETCH);
				pVLayout->AddWidget(pHLayout);

				Widgets::Button* pOkButton = new Widgets::Button(250, 50, 0, 0);
				Widgets::Label* pOkLabel = new Widgets::Label(0, 0, 1, "OK");
				pOkLabel->SetSizeStyle(Widgets::Widget::FIT);
				pOkLabel->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
				pOkButton->AddWidget(pOkLabel);
				pOkButton->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
				pOkButton->OnClick([](int, int) -> bool { Widgets::WidgetMgr::Get().CloseModalWindow(); return true; });
				pHLayout->AddWidget(pOkButton);

				Widgets::Button* pCancelButton = new Widgets::Button(250, 50, 0, 0);
				Widgets::Label* pCancelLabel = new Widgets::Label(0, 0, 1, "CANCEL");
				pCancelLabel->SetSizeStyle(Widgets::Widget::FIT);
				pCancelLabel->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
				pCancelButton->AddWidget(pCancelLabel);
				pCancelButton->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
				pCancelButton->OnClick([](int, int) -> bool { Widgets::WidgetMgr::Get().CloseModalWindow(); return true; });
				pHLayout->AddWidget(pCancelButton);

				Widgets::WidgetMgr::Get().OpenModalWindow(pWindow);
				return true;
			});
		
		m_pLabelContainer = new Widgets::Container();
		m_pLabelContainer->SetSizeStyle(Widgets::Widget::STRETCH);
		m_pLabelContainer->GetDefaultStyle().ShowBorder(true);
		m_pLabelContainer->GetDefaultStyle().SetBorderColor(DirectX::XMVectorSet(0.26f, 0.26f, 0.26f, 1.f));
		m_pLabelContainer->GetDefaultStyle().SetBackgroundColor(DirectX::XMVectorSet(0.22f, 0.22f, 0.22f, 1.f));
		AddWidget(m_pLabelContainer);

		m_pLabel = new Widgets::Label();
		m_pLabel->SetSizeStyle(Widgets::Widget::STRETCH);
		m_pLabel->SetX(5);
		m_pLabelContainer->AddWidget(m_pLabel);
	}

	AssetIdWidget::~AssetIdWidget()
	{}

	void AssetIdWidget::SetModel(const BaseModel* pModel)
	{
		m_pModel = pModel;
	}

	void AssetIdWidget::Update(uint64_t dt)
	{
		m_pLabel->SetText(m_pModel->GetData());
	}
}
