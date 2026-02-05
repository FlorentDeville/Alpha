/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/ArrayHeaderItem.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"
#include "Editors/Widgets/Dialog/ClassSelectionDialog.h"

#include "Systems/Objects/GameComponent.h"
#include "Systems/Objects/GameObject.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"

namespace Editors
{
	ArrayHeaderItem::ArrayHeaderItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index, bool enableAddElementButton)
		: PropertyGridItem(pObj, pField, index)
		, m_enableAddElementButton(enableAddElementButton)
	{
	}

	ArrayHeaderItem::~ArrayHeaderItem()
	{
	}

	void ArrayHeaderItem::CreateWidgets()
	{
		m_pNameWidget = CreatePodItemLabel();
	
		if (m_enableAddElementButton)
		{
			Widgets::Label* pLabel = new Widgets::Label("+");
			pLabel->SetSizeStyle(Widgets::Widget::FIT);
			pLabel->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

			Widgets::Button* pAddElementButton = new Widgets::Button(10, 20, 0, 0);
			pAddElementButton->AddWidget(pLabel);
			pAddElementButton->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
			pAddElementButton->OnClick([this]() { OnClick_AddArrayElement(); });

			m_pEditingWidget = pAddElementButton;
		}
	}

	void ArrayHeaderItem::OnClick_AddArrayElement()
	{
		if (!m_pField->GetType()->IsTemplateParamTypePointer())
		{
			ObjectWatcher::Get().AddArrayElement(m_pObj, m_pField, nullptr);
			return;
		}

		//here I know it's an array of pointers, so I need to add an element to the array and also to create the object
		const Systems::TypeDescriptor* pElementType = m_pField->GetType()->GetTemplateParamType();

		const Systems::TypeDescriptor* pElementBaseType = pElementType;
		while (pElementBaseType->GetBaseType())
			pElementBaseType = pElementBaseType->GetBaseType();

		// for now only support pointers to Object.
		assert(pElementBaseType->GetSid() == CONSTSID("Systems::Object"));

		//now let the user choose what class to instanciate.
		ClassSelectionDialog* pDialog = new ClassSelectionDialog(pElementType->GetSid());
		pDialog->Open();
		pDialog->OnOk([this](const Core::Sid& classNameSid)
			{
				const Systems::TypeDescriptor* pTypeToCreate = Systems::ReflectionMgr::Get().GetType(classNameSid);
				if (!pTypeToCreate)
					return;

				void* pNewItem = nullptr;

				if (pTypeToCreate->IsGameComponent())
				{
					Systems::GameComponent* pComponent = Systems::CreateNewGameComponent(pTypeToCreate);
					pComponent->SetOwner(static_cast<Systems::GameObject*>(m_pObj));
					pNewItem = pComponent;
				}
				else if (pTypeToCreate->IsGameObject())
					pNewItem = Systems::CreateNewGameObject(pTypeToCreate);
				else if (pTypeToCreate->IsObject())
					pNewItem = Systems::CreateObject(pTypeToCreate);
				else
					assert(false); // we can only create Object derived class here

				if (!pNewItem)
					return;

				ObjectWatcher::Get().AddArrayElement(m_pObj, m_pField, pNewItem);
			});
	}
}
