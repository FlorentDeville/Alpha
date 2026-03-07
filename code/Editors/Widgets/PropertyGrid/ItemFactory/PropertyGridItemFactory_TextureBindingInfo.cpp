/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/PropertyGrid/ItemFactory/PropertyGridItemFactory_TextureBindingInfo.h"

#include "Editors/Widgets/Dialog/AssetDialog.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/Material/TextureBindingInfo.h"
#include "Systems/Assets/AssetObjects/Texture/CubemapAsset.h"
#include "Systems/Assets/AssetObjects/Texture/Texture2DAsset.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/TextBox.h"

namespace Editors
{
	PropertyGridItemFactory_TextureBindingInfo::PropertyGridItemFactory_TextureBindingInfo()
		: PropertyGridItemFactory()
		, m_pTextbox(nullptr)
		, m_pBindingInfo(nullptr)
	{ }

	PropertyGridItemFactory_TextureBindingInfo::~PropertyGridItemFactory_TextureBindingInfo()
	{ }

	void PropertyGridItemFactory_TextureBindingInfo::CreateItems(void* pObj, const Core::FieldDescriptor* pField, uint32_t index)
	{
		if (pField->GetType()->IsContainer())
		{
			Core::BaseArray* pArray = pField->GetDataPtr<Core::BaseArray>(pObj);
			m_pBindingInfo = reinterpret_cast<Systems::TextureBindingInfo*>(pArray->GetElement(index));
		}
		else
		{
			m_pBindingInfo = pField->GetDataPtr<Systems::TextureBindingInfo>(pObj);
		}

		Widgets::Layout* pRootLayout = new Widgets::Layout(Widgets::Layout::Vertical, Widgets::Widget::STRETCH);

		Widgets::Layout* pLayout = new Widgets::Layout(Widgets::Layout::Horizontal_Reverse, Widgets::Widget::HSTRETCH_VFIT);

		Widgets::Label* pLabel = new Widgets::Label("...");
		pLabel->SetSizeStyle(Widgets::Widget::FIT);
		pLabel->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		Widgets::Button* pButton = new Widgets::Button(30, 20, 0, 0);
		pButton->SetSizeStyle(Widgets::Widget::DEFAULT);
		pButton->AddWidget(pLabel);
		pButton->OnClick([this]() { OpenDialogBox(); });

		pLayout->AddWidget(pButton);

		m_pTextbox = new Widgets::TextBox();
		m_pTextbox->SetReadOnly(true);
		//m_pTextbox->OnMouseDoubleClick([this](const Widgets::MouseEvent& ev) { OpenDialogBox(); });
		UpdateTextbox();

		pLayout->AddWidget(m_pTextbox);

		pRootLayout->AddWidget(pLayout);

		const int BUFFER_SIZE = 32;
		std::string itemName;
		itemName.resize(BUFFER_SIZE);

		if (m_pBindingInfo->m_type == Systems::Texture2D)
			snprintf(itemName.data(), BUFFER_SIZE, "%s (%s)", m_pBindingInfo->m_name.c_str(), "Texture2D");
		else if(m_pBindingInfo->m_type == Systems::Cubemap)
			snprintf(itemName.data(), BUFFER_SIZE, "%s (%s)", m_pBindingInfo->m_name.c_str(), "Cubemap");
		else
			snprintf(itemName.data(), BUFFER_SIZE, "%s (%s)", m_pBindingInfo->m_name.c_str(), "Unknown");

		PropertyGridItem* pItem = new PropertyGridItem(itemName, pRootLayout);
		m_pPopulator->AddPropertyGridItem(pItem);
	}

	void PropertyGridItemFactory_TextureBindingInfo::UpdateTextbox()
	{
		Systems::ITextureAsset* pTexture = m_pBindingInfo->m_texture.GetPtr();

		if (!pTexture)
		{
			m_pTextbox->SetText("Unknown");
			return;
		}

		Systems::NewAssetId id = pTexture->GetId();
		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(id);
		if (pMetadata)
			m_pTextbox->SetText(pMetadata->GetVirtualName() + " (" + pMetadata->GetAssetId().ToString() + ")");
		else
			m_pTextbox->SetText("Unknown");
	}

	void PropertyGridItemFactory_TextureBindingInfo::OpenDialogBox()
	{
		const Systems::NewAssetType* pAssetType = nullptr;
		if (m_pBindingInfo->m_type == Systems::Texture2D)
		{
			Core::Sid sid = Core::TypeResolver<Systems::Texture2DAsset>::GetTypenameSid();
			pAssetType = Systems::AssetMgr::Get().GetAssetTypeFromClassName(sid);
		}
		else if (m_pBindingInfo->m_type == Systems::Cubemap)
		{
			Core::Sid sid = Core::TypeResolver<Systems::CubemapAsset>::GetTypenameSid();
			pAssetType = Systems::AssetMgr::Get().GetAssetTypeFromClassName(sid);
		}

		if (!pAssetType)
			return;

		AssetDialog* pDialog = new AssetDialog(pAssetType->GetSid());
		pDialog->Open();
		pDialog->OnOk([this](Systems::NewAssetId id)
			{
				Systems::HardAssetRefRaw hardRef(id);
				hardRef.Load();

				m_pBindingInfo->m_texture = id;
				m_pBindingInfo->m_texture.Resolve();

				UpdateTextbox();

				//ObjectWatcher::OPERATION op = m_pField->GetType()->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;
				//ObjectWatcher::Get().SendFieldModifiedEvent()
			});
	}
}
