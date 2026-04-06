/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/PropertyGrid/ItemFactory/PropertyGridItemFactory_TextureBindingInfo.h"

#include "Editors/Widgets/Dialog/AssetDialog.h"
#include "Editors/Widgets/PropertyGrid/Items/HardAssetRefItem.h"
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
	{ }

	PropertyGridItemFactory_TextureBindingInfo::~PropertyGridItemFactory_TextureBindingInfo()
	{ }

	void PropertyGridItemFactory_TextureBindingInfo::CreateItems(void* pObj, const Core::FieldDescriptor* pField, uint32_t index)
	{
		Systems::TextureBindingInfo* pBindingInfo = nullptr;
		if (pField->GetType()->IsContainer())
		{
			Core::BaseArray* pArray = pField->GetDataPtr<Core::BaseArray>(pObj);
			pBindingInfo = reinterpret_cast<Systems::TextureBindingInfo*>(pArray->GetElement(index));
		}
		else
		{
			pBindingInfo = pField->GetDataPtr<Systems::TextureBindingInfo>(pObj);
		}

		Widgets::Layout* pRootLayout = new Widgets::Layout(Widgets::Layout::Vertical, Widgets::STRETCH);

		Widgets::Layout* pLayout = new Widgets::Layout(Widgets::Layout::Horizontal_Reverse, Widgets::HSTRETCH_VFIT);

		Widgets::Label* pLabel = new Widgets::Label("...");
		pLabel->SetSizeStyle(Widgets::FIT);
		pLabel->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		Widgets::TextBox* pTextbox = new Widgets::TextBox();
		pTextbox->SetReadOnly(true);
		UpdateTextbox(pBindingInfo, pTextbox);

		Widgets::Button* pButton = new Widgets::Button(30, 20, 0, 0);
		pButton->SetSizeStyle(Widgets::DEFAULT);
		pButton->AddWidget(pLabel);
		pButton->OnClick([pBindingInfo, pTextbox]() { OpenDialogBox(pBindingInfo, pTextbox); });

		pLayout->AddWidget(pButton);

		pLayout->AddWidget(pTextbox);

		pRootLayout->AddWidget(pLayout);

		const int BUFFER_SIZE = 32;
		std::string itemName;
		itemName.resize(BUFFER_SIZE);

		if (pBindingInfo->m_type == Systems::Texture2D)
			snprintf(itemName.data(), BUFFER_SIZE, "%s (%s)", pBindingInfo->m_name.c_str(), "Texture2D");
		else if(pBindingInfo->m_type == Systems::Cubemap)
			snprintf(itemName.data(), BUFFER_SIZE, "%s (%s)", pBindingInfo->m_name.c_str(), "Cubemap");
		else
			snprintf(itemName.data(), BUFFER_SIZE, "%s (%s)", pBindingInfo->m_name.c_str(), "Unknown");

		PropertyGridItem* pItem = new PropertyGridItem(itemName, pRootLayout);
		m_pPopulator->AddPropertyGridItem(pItem);
	}

	void PropertyGridItemFactory_TextureBindingInfo::UpdateTextbox(const Systems::TextureBindingInfo* pBindingInfo, Widgets::TextBox* pTextBox)
	{
		const Systems::ITextureAsset* pTexture = pBindingInfo->m_texture.GetPtr();

		if (!pTexture)
		{
			pTextBox->SetText("Unknown");
			return;
		}

		Systems::NewAssetId id = pTexture->GetId();
		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(id);
		if (pMetadata)
			pTextBox->SetText(pMetadata->GetVirtualName() + " (" + pMetadata->GetAssetId().ToString() + ")");
		else
			pTextBox->SetText("Unknown");
	}

	void PropertyGridItemFactory_TextureBindingInfo::OpenDialogBox(Systems::TextureBindingInfo* pBindingInfo, Widgets::TextBox* pTextBox)
	{
		const Systems::NewAssetType* pAssetType = nullptr;
		if (pBindingInfo->m_type == Systems::Texture2D)
		{
			Core::Sid sid = Core::TypeResolver<Systems::Texture2DAsset>::GetTypenameSid();
			pAssetType = Systems::AssetMgr::Get().GetAssetTypeFromClassName(sid);
		}
		else if (pBindingInfo->m_type == Systems::Cubemap)
		{
			Core::Sid sid = Core::TypeResolver<Systems::CubemapAsset>::GetTypenameSid();
			pAssetType = Systems::AssetMgr::Get().GetAssetTypeFromClassName(sid);
		}

		if (!pAssetType)
			return;

		AssetDialog* pDialog = new AssetDialog(pAssetType->GetSid());
		pDialog->Open();
		pDialog->OnOk([pBindingInfo, pTextBox](Systems::NewAssetId id)
			{
				Systems::HardAssetRef<Systems::ITextureAsset> hardRef(id);
				hardRef.Load(Systems::LoadingDomain::EDITOR);

				pBindingInfo->m_texture = hardRef;

				UpdateTextbox(pBindingInfo, pTextBox);
			});
	}
}
