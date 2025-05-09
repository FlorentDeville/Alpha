/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "Systems/Objects/Object.h"

#include "Widgets/TextBox.h"

namespace Editors
{
	PropertyGridPopulator::PropertyGridPopulator()
	{ }

	PropertyGridPopulator::~PropertyGridPopulator()
	{ }

	void PropertyGridPopulator::Populate(PropertyGridWidget* pWidget, Systems::Object* pObject)
	{
		const Systems::TypeDescriptor* pType = pObject->GetTypeDescriptor();

		const std::vector<Systems::FieldDescriptor>& fields = pType->GetFields();
		for (const Systems::FieldDescriptor& field : fields)
		{
			Widgets::Widget* pEditingWidget = nullptr;

			const Systems::TypeDescriptor* pFieldType = field.GetType();
			switch (pFieldType->GetSid())
			{
			case SID("std::string"):
			{
				Widgets::TextBox* pTextBox = new Widgets::TextBox();

				const std::string* pValue = field.GetDataPtr<std::string>(pObject);
				pTextBox->SetText(*pValue);
				
				pEditingWidget = pTextBox;
				
			}
			break;

			default:
				break;
			}

			PropertyGridItem* pItem = new PropertyGridItem(field.GetName(), pEditingWidget);
			pWidget->AddProperty(pItem);
		}
	}
}
