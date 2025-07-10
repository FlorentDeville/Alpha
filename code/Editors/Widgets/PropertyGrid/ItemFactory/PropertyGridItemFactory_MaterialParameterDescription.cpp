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
#include "Widgets/Widgets/SliderFloat.h"

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
		{
			float* pFloat = reinterpret_cast<float*>(pMatParamDesc->m_value.GetData());

			Widgets::Layout* pLayout = new Widgets::Layout();
			pLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
			pLayout->SetDirection(Widgets::Layout::Horizontal);
			pLayout->SetSpace(DirectX::XMINT2(SPACE, 0));
			pLayout->SetTransparent(true);

			pLayout->AddWidget(CreateSingleFloatWidget(pFloat));
			pLayout->AddWidget(CreateSingleFloatWidget(++pFloat));
			pLayout->AddWidget(CreateSingleFloatWidget(++pFloat));
			pLayout->AddWidget(CreateSingleFloatWidget(++pFloat));
			pWidget = pLayout;
		}
		break;

		case SID("color"):
		{
			float* pFloat = reinterpret_cast<float*>(pMatParamDesc->m_value.GetData());

			Widgets::Color currentColor(pFloat[0], pFloat[1], pFloat[2], pFloat[3]);

			Widgets::Layout* pLayout = new Widgets::Layout();
			pLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
			pLayout->SetDirection(Widgets::Layout::Horizontal);
			pLayout->SetSpace(DirectX::XMINT2(SPACE, 0));
			pLayout->SetTransparent(true);

			Widgets::SliderFloat* pRedChannel = new Widgets::SliderFloat(0, 1, pFloat[0]);
			pLayout->AddWidget(pRedChannel);

			Widgets::SliderFloat* pGreenChannel = new Widgets::SliderFloat(0, 1, pFloat[1]);
			pLayout->AddWidget(pGreenChannel);

			Widgets::SliderFloat* pBlueChannel = new Widgets::SliderFloat(0, 1, pFloat[2]);
			pLayout->AddWidget(pBlueChannel);

			Widgets::SliderFloat* pAlphaChannel = new Widgets::SliderFloat(0, 1, pFloat[2]);
			pLayout->AddWidget(pAlphaChannel);

			Widgets::Container* pColorWidget = new Widgets::Container(20, 20);
			pColorWidget->GetDefaultStyle().SetBackgroundColor(currentColor);
			pColorWidget->SetSizeStyle(Widgets::Widget::SIZE_STYLE::DEFAULT);
			pLayout->AddWidget(pColorWidget);

			pRedChannel->OnValidate([this, pColorWidget, pFloat](const float value)
				{
					pFloat[0] = value;
					m_pPopulator->SendDataChangedEvent();

					pColorWidget->GetDefaultStyle().GetBackgroundColor().m_channels[0] = value;
				});

			pGreenChannel->OnValidate([this, pColorWidget, pFloat](const float value)
				{
					pFloat[1] = value;
					m_pPopulator->SendDataChangedEvent();

					pColorWidget->GetDefaultStyle().GetBackgroundColor().m_channels[1] = value;
				});

			pBlueChannel->OnValidate([this, pColorWidget, pFloat](const float value)
				{
					pFloat[2] = value;
					m_pPopulator->SendDataChangedEvent();

					pColorWidget->GetDefaultStyle().GetBackgroundColor().m_channels[2] = value;
				});

			pAlphaChannel->OnValidate([this, pColorWidget, pFloat](const float value)
				{
					pFloat[3] = value;
					m_pPopulator->SendDataChangedEvent();

					pColorWidget->GetDefaultStyle().GetBackgroundColor().m_channels[3] = value;
				});

			pWidget = pLayout;
		}
		break;

		case SID("float3"):
		{
			float* pFloat = reinterpret_cast<float*>(pMatParamDesc->m_value.GetData());

			Widgets::Layout* pLayout = new Widgets::Layout();
			pLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
			pLayout->SetDirection(Widgets::Layout::Horizontal);
			pLayout->SetSpace(DirectX::XMINT2(SPACE, 0));
			pLayout->SetTransparent(true);

			pLayout->AddWidget(CreateSingleFloatWidget(pFloat));
			pLayout->AddWidget(CreateSingleFloatWidget(++pFloat));
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
