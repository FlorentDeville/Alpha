/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/HardAssetRefItem.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"
#include "Editors/Widgets/Dialog/AssetDialog.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetRef/HardAssetRef.h"
#include "Systems/Reflection/FieldDescriptor.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/TextBox.h"

namespace Editors
{
	HardAssetRefItem::HardAssetRefItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
		, m_pTextbox()
	{ }

	HardAssetRefItem::~HardAssetRefItem()
	{ }

	void HardAssetRefItem::CreateWidgets()
	{
		Widgets::Layout* pLayout = new Widgets::Layout(Widgets::Layout::Horizontal_Reverse, Widgets::Widget::HSTRETCH_VFIT);

		Widgets::Label* pLabel = new Widgets::Label("...");
		pLabel->SetSizeStyle(Widgets::Widget::FIT);
		pLabel->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		Widgets::Button* pButton = new Widgets::Button(30, 20, 0, 0);
		pButton->SetSizeStyle(Widgets::Widget::DEFAULT);
		pButton->AddWidget(pLabel);
		pButton->OnClick([this]()
			{
				AssetDialog* pDialog = new AssetDialog(Core::INVALID_SID);
				pDialog->Open();
				pDialog->OnOk([this](Systems::NewAssetId id)
					{
						ObjectWatcher::OPERATION op = m_pField->GetType()->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;
						ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &id);
					});
			});

		pLayout->AddWidget(pButton);

		m_pTextbox = new Widgets::TextBox();
		m_pTextbox->SetReadOnly(true);
		pLayout->AddWidget(m_pTextbox);

		UpdateValue();

		m_pNameWidget = CreateDefaultItemLabel();
		m_pEditingWidget = pLayout;
	}

	void HardAssetRefItem::UpdateValue()
	{
		Systems::NewAssetId* pId = m_pField->GetDataPtr<Systems::NewAssetId>(m_pObj);
		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(*pId);
		if (pMetadata)
			m_pTextbox->SetText(pMetadata->GetVirtualName() + " (" + pMetadata->GetAssetId().ToString() + ")");
		else
			m_pTextbox->SetText("Unknown");
	}
}
