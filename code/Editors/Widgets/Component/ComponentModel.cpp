/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Component/ComponentModel.h"

#include "Editors/Widgets/AssetId/AssetIdModel.h"
#include "Editors/Widgets/BaseModel.h"

namespace Editors
{
	std::string ComponentModel::s_defaultValue = "DEADMEAT";

	ComponentModel::ComponentModel(Component* pComponent)
		: m_pComponent(pComponent)
	{
		const int propertyCount = m_pComponent->GetPropertyCount();
		for (int ii = 0; ii < propertyCount; ++ii)
		{
			BaseModel* pNewModel = nullptr;
			Property* pProperty = m_pComponent->GetProperty(ii);
			PropertyType type = pProperty->GetType();
			switch (type)
			{
			case PropertyType::kAssetMaterial:
			case PropertyType::kAssetMesh:
			{
				pNewModel = new AssetIdModel(pProperty->GetValue<Systems::AssetId>(), type);
			}
			break;

			case PropertyType::kMat44f:
				break;

			default:
				assert(false);
				break;
			}
			Core::CallbackId cid = pProperty->OnValueChanged([this, ii](const PropertyValue& oldValue, const PropertyValue& newValue)
				{
					if (m_onPropertyValueChanged)
						m_onPropertyValueChanged(ii);
				});

			m_cidPropertyOnValueChanged.push_back(cid);
			m_modelsArray.push_back(pNewModel);
		}
	}

	ComponentModel::~ComponentModel()
	{
		const int propertyCount = m_pComponent->GetPropertyCount();
		for (int ii = 0; ii < propertyCount; ++ii)
		{
			Property* pProperty = m_pComponent->GetProperty(ii);
			pProperty->DisconnectOnValueChanged(m_cidPropertyOnValueChanged[ii]);
		}

		m_cidPropertyOnValueChanged.clear();
		m_modelsArray.clear();
	}

	int ComponentModel::GetPropertyCount() const
	{
		if (!m_pComponent)
			return 0;

		return m_pComponent->GetPropertyCount();
	}

	const std::string& ComponentModel::GetPropertyName(int row) const
	{
		if (!m_pComponent)
			return s_defaultValue;

		return m_pComponent->GetProperty(row)->GetName();
	}

	PropertyType ComponentModel::GetPropertyType(int row) const
	{
		if (!m_pComponent)
			return PropertyType::kUnknown;

		return m_pComponent->GetProperty(row)->GetType();
	}

	void ComponentModel::SetPropertyValue(int row, const Core::Mat44f& value)
	{
		Property* pProperty = m_pComponent->GetProperty(row);
		pProperty->SetMatrix(value);
	}

	BaseModel* ComponentModel::GetModel(int row) const
	{
		return m_modelsArray[row];
	}
}
