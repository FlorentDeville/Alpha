/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/ColorItem.h"

#include "Core/Color/Color.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/Reflection/FieldDescriptor.h"

#include "Widgets/Container.h"
#include "Widgets/Layout.h"
#include "Widgets/TextBox.h"
#include "Widgets/Widgets/SliderFloat.h"

namespace Editors
{
	ColorItem::ColorItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
		, m_pSliderRedChannel(nullptr)
		, m_pSliderGreenChannel(nullptr)
		, m_pSliderBlueChannel(nullptr)
		, m_pColorWidget(nullptr)
	{ }

	ColorItem::~ColorItem()
	{ }

	void ColorItem::CreateWidgets()
	{
		const int TEXT_BOX_WIDTH = 100;
		const int SPACE = 10;

		const Core::Color* pValue = GetDataPtr<Core::Color>();

		Widgets::Color currentColor(pValue->GetRed(), pValue->GetGreen(), pValue->GetBlue(), 1.f);

		Widgets::Layout* pLayout = new Widgets::Layout();
		pLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
		pLayout->SetDirection(Widgets::Layout::Horizontal);
		pLayout->SetSpace(DirectX::XMINT2(SPACE, 0));
		pLayout->GetDefaultStyle().SetBackgroundColor(Widgets::Color(0.f, 0.f, 0.f, 0.f));

		m_pSliderRedChannel = new Widgets::SliderFloat(0, 1, pValue->GetRed());
		pLayout->AddWidget(m_pSliderRedChannel);

		m_pSliderGreenChannel = new Widgets::SliderFloat(0, 1, pValue->GetGreen());
		pLayout->AddWidget(m_pSliderGreenChannel);

		m_pSliderBlueChannel = new Widgets::SliderFloat(0, 1, pValue->GetBlue());
		pLayout->AddWidget(m_pSliderBlueChannel);

		m_pColorWidget = new Widgets::Container(20, 20);
		m_pColorWidget->GetDefaultStyle().SetBackgroundColor(currentColor);
		m_pColorWidget->SetSizeStyle(Widgets::Widget::SIZE_STYLE::DEFAULT);
		pLayout->AddWidget(m_pColorWidget);

		m_pSliderRedChannel->OnValidate([this, pValue](const float value)
			{
				ObjectWatcher::OPERATION op = m_pField->GetType()->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;

				Core::Color copy = *pValue;
				copy.SetRed(value);
				ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &copy);
			});

		m_pSliderGreenChannel->OnValidate([this, pValue](const float value)
			{
				ObjectWatcher::OPERATION op = m_pField->GetType()->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;

				Core::Color copy = *pValue;
				copy.SetGreen(value);
				ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &copy);
			});

		m_pSliderBlueChannel->OnValidate([this, pValue](const float value)
			{
				ObjectWatcher::OPERATION op = m_pField->GetType()->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;

				Core::Color copy = *pValue;
				copy.SetBlue(value);
				ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &copy);
			});

		m_pEditingWidget = pLayout;

		m_pNameWidget = CreateDefaultItemLabel();
	}

	void ColorItem::UpdateValue()
	{
		const Core::Color* pValue = GetDataPtr<Core::Color>();
		m_pSliderRedChannel->SetValue(pValue->GetRed());
		m_pSliderGreenChannel->SetValue(pValue->GetGreen());
		m_pSliderBlueChannel->SetValue(pValue->GetBlue());

		Widgets::Color currentColor(pValue->GetRed(), pValue->GetGreen(), pValue->GetBlue(), 1.f);
		m_pColorWidget->GetDefaultStyle().SetBackgroundColor(currentColor);
	}
}
