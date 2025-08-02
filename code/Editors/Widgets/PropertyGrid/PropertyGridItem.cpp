/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

#include "Core/Collections/BaseArray.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"

#include "Systems/Objects/Object.h"
#include "Systems/Reflection/FieldDescriptor.h"

#include "Widgets/Button.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	PropertyGridItem::PropertyGridItem()
		: m_pEditingWidget(nullptr)
		, m_pNameWidget(nullptr)
		, m_pObj(nullptr)
		, m_pField(nullptr)
		, m_index(-1)
	{ }

	PropertyGridItem::PropertyGridItem(const std::string& name, Widgets::Widget* pEditingWidget)
		: m_pEditingWidget(pEditingWidget)
		, m_pObj(nullptr)
		, m_pField(nullptr)
		, m_index(-1)
	{
		m_pNameWidget = new Widgets::Label(name);
	}

	PropertyGridItem::PropertyGridItem(Widgets::Widget* pNameWidget, Widgets::Widget* pEditingWidget)
		: m_pNameWidget(pNameWidget)
		, m_pEditingWidget(pEditingWidget)
		, m_pObj(nullptr)
		, m_pField(nullptr)
		, m_index(-1)
	{ }

	PropertyGridItem::PropertyGridItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
		: m_pNameWidget(nullptr)
		, m_pEditingWidget(nullptr)
		, m_pObj(pObj)
		, m_pField(pField)
		, m_index(index)
	{ }

	Widgets::Widget* PropertyGridItem::GetNameWidget() const
	{
		return m_pNameWidget;
	}

	Widgets::Widget* PropertyGridItem::GetEditingWidget() const
	{
		return m_pEditingWidget;
	}

	void PropertyGridItem::CreateWidgets()
	{ }

	void PropertyGridItem::UpdateValue()
	{ }

	bool PropertyGridItem::IsField(const Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index) const
	{
		return m_pObj == pObj && m_pField == pField && m_index == index;
	}

	Widgets::Widget* PropertyGridItem::CreateDefaultItemLabel()
	{
		if (m_pField->IsContainer())
			return CreateArrayItemLabel();
		else
			return CreatePodItemLabel();
	}

	Widgets::Widget* PropertyGridItem::CreatePodItemLabel()
	{
		return new Widgets::Label(m_pField->GetName());
	}

	Widgets::Widget* PropertyGridItem::CreateArrayItemLabel()
	{
		assert(m_pField->IsContainer());

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
						ObjectWatcher::Get().RemoveArrayElement(m_pObj, m_pField, m_index);
					});

				pDialog->Open();
			});

		pNameLayout->AddWidget(pDeleteButton);

		const int BUFFER_SIZE = 64;
		char buffer[BUFFER_SIZE] = { '\0' };

		if (m_pField->GetElementType()->IsObject())
		{
			Core::BaseArray* pArray = m_pField->GetDataPtr<Core::BaseArray>(m_pObj);

			const Systems::Object* pElement = static_cast<const Systems::Object*>(pArray->GetConstElement(m_index));
			const Systems::TypeDescriptor* pObjectType = pElement->GetTypeDescriptor();
			sprintf_s(buffer, "[%d] %s", m_index, pObjectType->GetName().c_str());
		}
		else
		{
			sprintf_s(buffer, "[%d]", m_index);
		}

		Widgets::Label* pNameLabel = new Widgets::Label(buffer);
		pNameLabel->SetSizeStyle(Widgets::Widget::HSIZE_FIT | Widgets::Widget::VSIZE_DEFAULT);
		pNameLayout->AddWidget(pNameLabel);

		return pNameLayout;
	}
}
