/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/ComponentRefItem.h"

#include "Core/Reflection/FieldDescriptor.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"
#include "Editors/Widgets/Dialog/Model/ComponentSelectionModel.h"
#include "Editors/Widgets/Dialog/TableDialog.h"

#include "Systems/GameComponent/ComponentRef/ComponentRefRaw.h"
#include "Systems/Objects/GameObject.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Models/ModelIndex.h"
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
				ComponentSelectionModel* pModel = new ComponentSelectionModel(m_pOwner, Core::Sid());
				TableDialog* pDialog = new TableDialog("Select component", pModel);
				pDialog->SetColumnWidth(0, 50);
				pDialog->SetColumnWidth(1, 300);
				pDialog->SetColumnWidth(2, 135);
				pDialog->Open();
				pDialog->OnOk([this, pModel](Widgets::ModelIndex index) 
					{
						Core::Guid componentGuid = pModel->GetGuid(index);
						ObjectWatcher::OPERATION op = m_pField->GetType()->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;

						Systems::ComponentRefRaw componentRefRaw(componentGuid);
						ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &componentRefRaw);
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

	void ComponentRefItem::UpdateValue()
	{
		Systems::ComponentRefRaw* pRaw = m_pField->GetDataPtr<Systems::ComponentRefRaw>(m_pObj);

		const Systems::GameComponent* pComponent = m_pOwner->FindComponent(pRaw->GetGuid());
		if (!pComponent)
		{
			m_pTextbox->SetText("Empty");
		}
		else
		{
			std::string guidString;
			guidString.resize(37, '\0');
			pRaw->GetGuid().ToString(guidString.data(), static_cast<uint32_t>(guidString.size()));

			std::string typeName = pComponent->GetTypeDescriptor()->GetName();
			{
				
				size_t pos = typeName.find_last_of(':');
				if (pos != std::string::npos)
					typeName = typeName.substr(pos + 1);
			}

			const uint32_t SIZE = 256;
			std::string displayText;
			displayText.resize(SIZE, '\0');
			snprintf(displayText.data(), SIZE, "%s (%s)", typeName.c_str(), guidString.c_str());
			m_pTextbox->SetText(displayText);
		}
	}
}
