/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/ItemFactory/PropertyGridItemFactory_MaterialParameterDescription.h"

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "Systems/Assets/AssetObjects/Material/MaterialParameterDescription.h"

#include "Widgets/Layout.h"
#include "Widgets/TextBox.h"

#include <cstdlib>

namespace Editors
{
	const int TEXT_BOX_WIDTH = 100;
	const int SPACE = 10;

	PropertyGridItemFactory_MaterialParameterDescription::PropertyGridItemFactory_MaterialParameterDescription()
		: PropertyGridItemFactory()
	{ }

	PropertyGridItemFactory_MaterialParameterDescription::~PropertyGridItemFactory_MaterialParameterDescription()
	{ }

	void PropertyGridItemFactory_MaterialParameterDescription::CreateItems(const Systems::TypeDescriptor* pType, void* pData, int depth)
	{
		Systems::MaterialParameterDescription* pMatParamDesc = reinterpret_cast<Systems::MaterialParameterDescription*>(pData);

		Widgets::Widget* pWidget = nullptr;

		if (pMatParamDesc->m_size != pMatParamDesc->m_value.GetSize())
			pMatParamDesc->m_value.Resize(pMatParamDesc->m_size);

		switch (pMatParamDesc->m_type)
		{
		case SID("float4"):
		case SID("color"):
		{
			float* pFloat = reinterpret_cast<float*>(pMatParamDesc->m_value.GetData());

			Widgets::Layout* pLayout = new Widgets::Layout();
			pLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
			pLayout->SetDirection(Widgets::Layout::Horizontal);

			pLayout->AddWidget(CreateSingleFloatWidget(pFloat));
			pLayout->AddWidget(new Widgets::Container(SPACE, 20));
			pLayout->AddWidget(CreateSingleFloatWidget(++pFloat));
			pLayout->AddWidget(new Widgets::Container(SPACE, 20));
			pLayout->AddWidget(CreateSingleFloatWidget(++pFloat));
			pLayout->AddWidget(new Widgets::Container(SPACE, 20));
			pLayout->AddWidget(CreateSingleFloatWidget(++pFloat));
			pWidget = pLayout;
		}
		break;

		case SID("float3"):
		{
			float* pFloat = reinterpret_cast<float*>(pMatParamDesc->m_value.GetData());

			Widgets::Layout* pLayout = new Widgets::Layout();
			pLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
			pLayout->SetDirection(Widgets::Layout::Horizontal);

			pLayout->AddWidget(CreateSingleFloatWidget(pFloat));
			pLayout->AddWidget(new Widgets::Container(SPACE, 20));
			pLayout->AddWidget(CreateSingleFloatWidget(++pFloat));
			pLayout->AddWidget(new Widgets::Container(SPACE, 20));
			pLayout->AddWidget(CreateSingleFloatWidget(++pFloat));
			pWidget = pLayout;
		}
		break;

		case SID("float"):
		{
			float* pFloat = reinterpret_cast<float*>(pMatParamDesc->m_value.GetData());
			pWidget = CreateSingleFloatWidget(pFloat);
		}
		break;

		default:
			break;
		}

		

		PropertyGridItem* pItem = new PropertyGridItem(pMatParamDesc->m_name, pWidget);
		m_pPropertyGridWidget->AddProperty(pItem, depth);
	}

	Widgets::TextBox* PropertyGridItemFactory_MaterialParameterDescription::CreateSingleFloatWidget(float* pData)
	{
		Widgets::TextBox* pWidget = new Widgets::TextBox();
		pWidget->SetSize(DirectX::XMUINT2(TEXT_BOX_WIDTH, 20));
		pWidget->SetSizeStyle(Widgets::Widget::SIZE_STYLE::DEFAULT);

		const int BUFFER_SIZE = 32;
		char buffer[BUFFER_SIZE] = { '\0' };
		sprintf_s(buffer, BUFFER_SIZE, "%f", *pData);
		pWidget->SetText(buffer);

		pWidget->OnValidate([this, pData](const std::string& value)
			{
				char* pEnd = nullptr;

				float f = std::strtof(value.c_str(), &pEnd);
				if (value.c_str() != pEnd)
				{
					*pData = f;
					m_pPopulator->SendDataChangedEvent();
				}
			});

		return pWidget;
	}
}
