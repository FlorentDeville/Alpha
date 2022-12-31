/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/AssetId/AssetIdWidget.h"

#include "Editors/Widgets/AssetId/AssetIdModel.h"

#include "Systems/Assets/AssetId.h"

#include "Widgets/Button.h"
#include "Widgets/Container.h"
#include "Widgets/Label.h"

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
