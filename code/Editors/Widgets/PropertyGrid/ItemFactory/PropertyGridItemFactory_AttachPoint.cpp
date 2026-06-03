/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/PropertyGrid/ItemFactory/PropertyGridItemFactory_AttachPoint.h"

#include "Editors/Widgets/PropertyGrid/Items/ArrayElementHeaderItem.h"
#include "Editors/Widgets/PropertyGrid/Items/SqtItem.h"
#include "Editors/Widgets/PropertyGrid/Items/StringItem.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"

#include "Systems/Assets/AssetObjects/Mesh/AttachPoint.h"

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

		//add array item header
		PropertyGridPopulator::ParentItemContextScope* pJanitor = nullptr;
		if (pField->GetType()->IsContainer())
		{
			ArrayElementHeaderItem* pHeader = new ArrayElementHeaderItem(pObj, pField, index);
			m_pPopulator->AddPropertyGridItem(pHeader);

			//not great but it works
			pJanitor = new PropertyGridPopulator::ParentItemContextScope(pHeader, m_pPopulator);
		}

		//add item for the locator
		SqtItem* pSqtItem = new SqtItem(pAttachPoint, pAttachPointType->FindField("m_locator"), 0);
		m_pPopulator->AddPropertyGridItem(pSqtItem);
		
		//add item for the name with logic to change the name sid
		StringItem* pStringItem = new StringItem(pAttachPoint, pAttachPointType->FindField("m_name"), 0);
		pStringItem->OnAfterValueUpdated([pAttachPoint](const void* pValue)
			{
				pAttachPoint->UpdateNameSid();
			});

		m_pPopulator->AddPropertyGridItem(pStringItem);

		if (pJanitor)
			delete pJanitor;
	}
}
