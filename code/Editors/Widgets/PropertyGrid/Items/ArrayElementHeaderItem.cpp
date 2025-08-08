/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/ArrayElementHeaderItem.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"

#include "Systems/Objects/Object.h"

#include "Widgets/Button.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	ArrayElementHeaderItem::ArrayElementHeaderItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
	{
	}

	ArrayElementHeaderItem::~ArrayElementHeaderItem()
	{
	}

	void ArrayElementHeaderItem::CreateWidgets()
	{
		m_pEditingWidget = nullptr;

		Widgets::Layout* pNameLayout = new Widgets::Layout(Widgets::Layout::Horizontal, Widgets::Widget::FIT);
		pNameLayout->GetDefaultStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
		pNameLayout->GetHoverStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
		pNameLayout->SetSpace(DirectX::XMINT2(5, 0));

		const Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::Get();
		Rendering::TextureId deleteTextureId = widgetMgr.GetIconTextureId(Widgets::IconId::kIconClose);
		Widgets::Icon* pDeleteIcon = new Widgets::Icon(deleteTextureId);

		Widgets::Button* pDeleteButton = new Widgets::Button(12, 12, 0, 0);
		pDeleteButton->AddWidget(pDeleteIcon);
		pDeleteButton->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::NONE, Widgets::Widget::VPOSITION_STYLE::MIDDLE);
		pDeleteButton->OnClick([this]()
			{
				Editors::OkCancelDialog* pDialog = new Editors::OkCancelDialog("Delete element", "Are you sure you want to delete this element from the array ?");
				pDialog->OnOk([this]()
					{
						ObjectWatcher::Get().ModifyField(m_pObj, m_pField, ObjectWatcher::REMOVE_ELEMENT, m_index, nullptr);
					});

				pDialog->Open();
			});

		pNameLayout->AddWidget(pDeleteButton);

		const int BUFFER_SIZE = 64;
		char buffer[BUFFER_SIZE] = { '\0' };

		const Systems::TypeDescriptor* pArrayType = m_pField->GetType();
		const Systems::TypeDescriptor* pArrayElementType = pArrayType->GetElementType();

		bool elementIsObject = pArrayElementType->IsObject();
		if (elementIsObject)
		{
			if (pArrayType->IsElementPointer())
			{
				const Core::BaseArray* pArray = m_pField->GetDataPtr<const Core::BaseArray>(m_pObj);
				const void* pElement = pArray->GetConstElement(m_index);
				const Systems::Object* const* pObject = reinterpret_cast<const Systems::Object* const*>(pElement);
				const Systems::TypeDescriptor* pObjectType = (*pObject)->GetTypeDescriptor();
				sprintf_s(buffer, "[%d] %s", m_index, pObjectType->GetName().c_str());
			}
			else
			{
				sprintf_s(buffer, "[%d] %s", m_index, pArrayElementType->GetName().c_str());
			}
		}
		else
		{
			sprintf_s(buffer, "[%d]", m_index);
		}

		Widgets::Label* pNameLabel = new Widgets::Label(buffer);
		pNameLabel->SetSizeStyle(Widgets::Widget::HSIZE_FIT | Widgets::Widget::VSIZE_DEFAULT);
		pNameLayout->AddWidget(pNameLabel);

		m_pNameWidget = pNameLayout;
	}
}
