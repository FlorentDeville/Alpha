/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/ComponentRefItem.h"

#include "Core/Reflection/FieldDescriptor.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/GameComponent/ComponentRef/ComponentRefRaw.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/TextBox.h"

namespace Editors
{
	ComponentRefItem::ComponentRefItem(void* pObj, const Core::FieldDescriptor* pField, uint32_t index, const Systems::GameObject* pOwner)
		: PropertyGridItem(pObj, pField, index)
		, m_pTextbox()
		, m_pOwner(pOwner)
	{ }

	ComponentRefItem::~ComponentRefItem()
	{ }

	void ComponentRefItem::CreateWidgets()
	{
		Widgets::Layout* pLayout = new Widgets::Layout(Widgets::Layout::Horizontal_Reverse, Widgets::HSTRETCH_VFIT);

		Widgets::Label* pLabel = new Widgets::Label("...");
		pLabel->SetSizeStyle(Widgets::FIT);
		pLabel->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		Widgets::Button* pButton = new Widgets::Button(30, 20, 0, 0);
		pButton->SetSizeStyle(Widgets::DEFAULT);
		pButton->AddWidget(pLabel);
		pButton->OnClick([this]()
			{
				/*const Core::TypeDescriptor* pTemplateParamType = m_pField->GetType()->GetTemplateParamType();
				const Systems::NewAssetType* pAssetType = Systems::AssetMgr::Get().GetAssetTypeFromClassName(pTemplateParamType->GetSid());
				if (!pAssetType)
					return;

				AssetDialog* pDialog = new AssetDialog(pAssetType->GetSid());
				pDialog->Open();
				pDialog->OnOk([this](Systems::NewAssetId id)
					{
						ObjectWatcher::OPERATION op = m_pField->GetType()->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;

						Systems::HardAssetRefRaw hardRef(id);
						hardRef.Load(Systems::LoadingDomain::EDITOR);

						ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &hardRef);
					});*/
			});

		pLayout->AddWidget(pButton);

		m_pTextbox = new Widgets::TextBox();
		m_pTextbox->SetReadOnly(true);
		pLayout->AddWidget(m_pTextbox);

		UpdateValue();

		m_pNameWidget = CreateDefaultItemLabel();
		m_pEditingWidget = pLayout;
	}

	void ComponentRefItem::UpdateValue()
	{
		Systems::ComponentRefRaw* pRaw = m_pField->GetDataPtr<Systems::ComponentRefRaw>(m_pObj);

		std::string guidString;
		guidString.resize(37, '\0');
		pRaw->GetGuid().ToString(guidString.data(), static_cast<uint32_t>(guidString.size()));
		m_pTextbox->SetText(guidString);
	}
}
