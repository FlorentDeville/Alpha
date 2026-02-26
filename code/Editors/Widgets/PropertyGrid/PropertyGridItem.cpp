/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

#include "Core/Collections/BaseArray.h"

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
	PropertyGridItem::PropertyGridItem()
		: m_pEditingWidget(nullptr)
		, m_pNameWidget(nullptr)
		, m_pObj(nullptr)
		, m_pField(nullptr)
		, m_index(-1)
		, m_pFieldName(nullptr)
	{ }

	PropertyGridItem::PropertyGridItem(const std::string& name, Widgets::Widget* pEditingWidget)
		: m_pEditingWidget(pEditingWidget)
		, m_pObj(nullptr)
		, m_pField(nullptr)
		, m_index(-1)
		, m_pFieldName(nullptr)
	{
		m_pNameWidget = new Widgets::Label(name);
	}

	PropertyGridItem::PropertyGridItem(Widgets::Widget* pNameWidget, Widgets::Widget* pEditingWidget)
		: m_pNameWidget(pNameWidget)
		, m_pEditingWidget(pEditingWidget)
		, m_pObj(nullptr)
		, m_pField(nullptr)
		, m_index(-1)
		, m_pFieldName(nullptr)
	{ }

	PropertyGridItem::PropertyGridItem(void* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
		: m_pNameWidget(nullptr)
		, m_pEditingWidget(nullptr)
		, m_pObj(pObj)
		, m_pField(pField)
		, m_index(index)
		, m_pFieldName(nullptr)
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

	bool PropertyGridItem::IsField(const void* pObj, const Systems::FieldDescriptor* pField, uint32_t index) const
	{
		return m_pObj == pObj && m_pField == pField && m_index == index;
	}

	void PropertyGridItem::ChangeIndex(uint32_t newIndex)
	{
		assert(m_pField->GetType()->IsContainer()); //changing index only makes sense in arrays
		m_index = newIndex;

		ArrayItemLabel_Init();
	}

	uint32_t PropertyGridItem::GetIndex() const
	{
		return m_index;
	}

	Widgets::Widget* PropertyGridItem::CreateDefaultItemLabel()
	{
		if (m_pField->GetType()->IsContainer())
			return CreateArrayItemLabel();
		else
			return CreatePodItemLabel();
	}

	Widgets::Widget* PropertyGridItem::CreatePodItemLabel()
	{
		m_pFieldName = new Widgets::Label(m_pField->GetName());
		return m_pFieldName;
	}

	Widgets::Widget* PropertyGridItem::CreateArrayItemLabel()
	{
		assert(m_pField->GetType()->IsContainer());

		Widgets::Layout* pNameLayout = new Widgets::Layout(Widgets::Layout::Horizontal, Widgets::Widget::FIT);
		pNameLayout->GetDefaultStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
		pNameLayout->GetHoverStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
		pNameLayout->SetSpace(DirectX::XMINT2(5, 0));

		const Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::Get();
		Rendering::TextureId deleteTextureId = widgetMgr.GetIconTextureId(Widgets::IconId::kIconClose);
		Widgets::Icon* pDeleteIcon = new Widgets::Icon(deleteTextureId);

		m_pDeleteButton = new Widgets::Button(12, 12, 0, 0);
		m_pDeleteButton->AddWidget(pDeleteIcon);
		m_pDeleteButton->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::NONE, Widgets::Widget::VPOSITION_STYLE::MIDDLE);
		
		pNameLayout->AddWidget(m_pDeleteButton);

		m_pFieldName = new Widgets::Label();
		m_pFieldName->SetSizeStyle(Widgets::Widget::HSIZE_FIT | Widgets::Widget::VSIZE_DEFAULT);
		pNameLayout->AddWidget(m_pFieldName);

		ArrayItemLabel_Init();

		return pNameLayout;
	}

	void PropertyGridItem::ArrayItemLabel_Init()
	{
		m_pDeleteButton->ClearOnClick();
		m_pDeleteButton->OnClick([this]()
			{
				Editors::OkCancelDialog* pDialog = new Editors::OkCancelDialog("Delete element", "Are you sure you want to delete this element from the array ?");
				pDialog->OnOk([this]()
					{
						ObjectWatcher::Get().RemoveArrayElement(m_pObj, m_pField, m_index);
					});

				pDialog->Open();
			});

		const int BUFFER_SIZE = 64;
		char buffer[BUFFER_SIZE] = { '\0' };

		if (m_pField->GetType()->GetTemplateParamType()->IsObject())
		{
			assert(m_pField->GetType()->IsTemplateParamTypePointer());

			Core::BaseArray* pArray = m_pField->GetDataPtr<Core::BaseArray>(m_pObj);

			const uint64_t* pPtr = reinterpret_cast<const uint64_t*>(pArray->GetConstElement(m_index));
			const Systems::Object* pElement = reinterpret_cast<const Systems::Object*>(*pPtr);
			const Systems::TypeDescriptor* pObjectType = pElement->GetTypeDescriptor();
			sprintf_s(buffer, "[%d] %s", m_index, pObjectType->GetName().c_str());
		}
		else
		{
			sprintf_s(buffer, "[%d]", m_index);
		}

		m_pFieldName->SetText(buffer);
	}
}
