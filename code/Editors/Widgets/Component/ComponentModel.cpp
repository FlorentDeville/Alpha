/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Component/ComponentModel.h"

#include "Editors/Widgets/BaseModel.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetMgr.h"

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

	std::string ComponentModel::ToString(Systems::AssetId id) const
	{
		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		const Systems::Asset* pAsset = assetMgr.GetAsset(id);
		if (!pAsset)
			return "Unknown Asset";
		else
			return pAsset->GetVirtualName() + " (" + id.ToString() + ")";
	}

	void ComponentModel::SetPropertyValue(int row, const Core::Mat44f& value)
	{
		Property* pProperty = m_pComponent->GetProperty(row);
		pProperty->SetMatrix(value);
	}

	void ComponentModel::SetPropertyValue(int row, Systems::AssetId value, Systems::AssetType type)
	{
		Property* pProperty = m_pComponent->GetProperty(row);
		switch (type)
		{
		case Systems::kMaterial:
			pProperty->SetMaterialAssetId(value);
			break;

		case Systems::kMesh:
			pProperty->SetMeshAssetId(value);
			break;

		default:
			assert(false);
			break;
		}
	}

	BaseModel* ComponentModel::GetModel(int row) const
	{
		return m_modelsArray[row];
	}
}
