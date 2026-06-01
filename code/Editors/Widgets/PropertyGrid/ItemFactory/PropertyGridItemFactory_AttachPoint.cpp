/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/PropertyGrid/ItemFactory/PropertyGridItemFactory_AttachPoint.h"

#include "Editors/Widgets/PropertyGrid/Items/SqtItem.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"

#include "Systems/Assets/AssetObjects/Mesh/AttachPoint.h"

#include "Widgets/TextBox.h"

namespace Editors
{
	PropertyGridItemFactory_AttachPoint::PropertyGridItemFactory_AttachPoint()
		: PropertyGridItemFactory()
	{ }

	PropertyGridItemFactory_AttachPoint::~PropertyGridItemFactory_AttachPoint()
	{ }

	void PropertyGridItemFactory_AttachPoint::CreateItems(void* pObj, const Core::FieldDescriptor* pField, uint32_t index)
	{
		Systems::AttachPoint* pAttachPoint = nullptr;
		const Core::TypeDescriptor* pAttachPointType = nullptr;
		if (pField->GetType()->IsContainer())
		{
			Core::BaseArray* pArray = pField->GetDataPtr<Core::BaseArray>(pObj);
			pAttachPoint = reinterpret_cast<Systems::AttachPoint*>(pArray->GetElement(index));

			pAttachPointType = pField->GetType()->GetTemplateParamType();
		}
		else
		{
			pAttachPoint = pField->GetDataPtr<Systems::AttachPoint>(pObj);
			pAttachPointType = pField->GetType();
		}

		if (!pAttachPoint)
			return;

		//add item for the locator
		SqtItem* pSqtItem = new SqtItem(pAttachPoint, pAttachPointType->FindField("m_locator"), 0);
		m_pPopulator->AddPropertyGridItem(pSqtItem);
		
		//add item for the name with logic to change the name sid
		Widgets::TextBox* pNameTextbox = new Widgets::TextBox();
		pNameTextbox->SetText(pAttachPoint->GetName());
		PropertyGridItem* pNameItem = new PropertyGridItem("name", pNameTextbox);
		m_pPopulator->AddPropertyGridItem(pNameItem);
	}
}
