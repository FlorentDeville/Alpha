/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/ItemFactory/PropertyGridItemFactory_MaterialParameterDescription.h"

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "Systems/Assets/AssetObjects/Material/MaterialParameterDescription.h"

#include "Widgets/Layout.h"
#include "Widgets/TextBox.h"

namespace Editors
{
	const int TEXT_BOX_WIDTH = 40;
	const int SPACE = 10;
	Widgets::TextBox* CreateSingleFloatWidget()
	{
		
		Widgets::TextBox* pWidget = new Widgets::TextBox();
		pWidget->SetSize(DirectX::XMUINT2(TEXT_BOX_WIDTH, 20));
		pWidget->SetSizeStyle(Widgets::Widget::SIZE_STYLE::DEFAULT);

		return pWidget;
	}

	PropertyGridItemFactory_MaterialParameterDescription::PropertyGridItemFactory_MaterialParameterDescription()
		: PropertyGridItemFactory()
	{ }

	PropertyGridItemFactory_MaterialParameterDescription::~PropertyGridItemFactory_MaterialParameterDescription()
	{ }

	void PropertyGridItemFactory_MaterialParameterDescription::CreateItems(const Systems::TypeDescriptor* pType, void* pData, int depth)
	{
		Systems::MaterialParameterDescription* pMatParamDesc = reinterpret_cast<Systems::MaterialParameterDescription*>(pData);

		Widgets::Widget* pWidget = nullptr;

		switch (pMatParamDesc->m_type)
		{
		case SID("float4"):
		case SID("color"):
		{
			Widgets::Layout* pLayout = new Widgets::Layout();
			pLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
			pLayout->SetDirection(Widgets::Layout::Horizontal);

			pLayout->AddWidget(CreateSingleFloatWidget());
			pLayout->AddWidget(new Widgets::Container(SPACE, 20));
			pLayout->AddWidget(CreateSingleFloatWidget());
			pLayout->AddWidget(new Widgets::Container(SPACE, 20));
			pLayout->AddWidget(CreateSingleFloatWidget());
			pLayout->AddWidget(new Widgets::Container(SPACE, 20));
			pLayout->AddWidget(CreateSingleFloatWidget());
			pWidget = pLayout;
		}
		break;

		case SID("float3"):
		{
			Widgets::Layout* pLayout = new Widgets::Layout();
			pLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
			pLayout->SetDirection(Widgets::Layout::Horizontal);

			pLayout->AddWidget(CreateSingleFloatWidget());
			pLayout->AddWidget(new Widgets::Container(SPACE, 20));
			pLayout->AddWidget(CreateSingleFloatWidget());
			pLayout->AddWidget(new Widgets::Container(SPACE, 20));
			pLayout->AddWidget(CreateSingleFloatWidget());
			pWidget = pLayout;
		}
		break;

		case SID("float"):
		{
			pWidget = CreateSingleFloatWidget();
		}
		break;

		default:
			break;
		}

		

		PropertyGridItem* pItem = new PropertyGridItem(pMatParamDesc->m_name, pWidget);
		m_pPropertyGridWidget->AddProperty(pItem, depth);


	}
}
