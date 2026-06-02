/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/PropertyGrid/ItemFactory/PropertyGridItemFactory_Transform.h"

#include "Editors/Widgets/PropertyGrid/Items/SqtItem.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"

#include "Systems/GameComponent/TransformComponent.h"

namespace Editors
{
	PropertyGridItemFactory_Transform::PropertyGridItemFactory_Transform()
		: PropertyGridItemFactory()
	{
	}

	PropertyGridItemFactory_Transform::~PropertyGridItemFactory_Transform()
	{
	}

	void PropertyGridItemFactory_Transform::CreateItems(void* pObj, const Core::FieldDescriptor* pField, uint32_t index)
	{
		Systems::TransformComponent* pTransform = nullptr;
		const Core::TypeDescriptor* pType = nullptr;
		if (pField->GetType()->IsContainer())
		{
			Core::BaseArray* pArray = pField->GetDataPtr<Core::BaseArray>(pObj);
			pTransform = reinterpret_cast<Systems::TransformComponent*>(pArray->GetElement(index));
			pType = pField->GetType()->GetTemplateParamType();
		}
		else
		{
			pTransform = pField->GetDataPtr<Systems::TransformComponent>(pObj);
			pType = pField->GetType();
		}

		if (!pTransform)
			return;

		//add item for the locator
		SqtItem* pSqtItem = new SqtItem(pTransform, pType->FindField("m_localSqt"), 0);
		pSqtItem->OnValueUpdated([pTransform](const void*) { pTransform->SetIsDirty(); });
		m_pPopulator->AddPropertyGridItem(pSqtItem);
	}
}
