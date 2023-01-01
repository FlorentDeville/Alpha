/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Component/ComponentModel.h"

#include "Editors/Widgets/AssetId/AssetIdModel.h"
#include "Editors/Widgets/Matrix/MatrixModel.h"

#include "Editors/LevelEditor/Component.h"

namespace Editors
{
	std::string ComponentModel::s_defaultValue = "DEADMEAT";

	ComponentModel::ComponentModel(Component* pComponent)
		: BaseModel()
		, m_pComponent(pComponent)
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
				PropertyValueAssetId& propertyAssetId = static_cast<PropertyValueAssetId&>(pProperty->GetValue());
				pNewModel = new AssetIdModel(propertyAssetId.Get(), type);
			}
			break;

			case PropertyType::kMat44f:
			{
				PropertyValueMat44f& propertyAssetId = static_cast<PropertyValueMat44f&>(pProperty->GetValue());
				pNewModel = new MatrixModel(&propertyAssetId.Get());
			}
			break;
				
			default:
				assert(false);
				break;
			}

			m_models.push_back(pNewModel);
		}
	}

	ComponentModel::~ComponentModel()
	{
		for (const BaseModel* pModel : m_models)
			delete pModel;

		m_models.clear();
	}

	int ComponentModel::GetRowCount() const
	{
		return m_pComponent->GetPropertyCount();
	}

	int ComponentModel::GetColumnCount() const
	{
		return 2;
	}

	const std::string& ComponentModel::GetData(int rowId, int columnId) const
	{
		if (columnId == 0) //property name
		{
			return m_pComponent->GetProperty(rowId)->GetName();
		}
		else if (columnId == 1) //value
		{
			const int propertyCount = m_pComponent->GetPropertyCount();
			for (int ii = 0; ii < propertyCount; ++ii)
			{
				return m_models[ii]->GetData();
			}
		}

		return s_defaultValue;
	}

	PropertyType ComponentModel::GetDataType(int rowId, int columnId) const
	{
		return m_pComponent->GetProperty(rowId)->GetType();
	}

	BaseModel* ComponentModel::GetSubModel(int rowId, int columnId)
	{
		return m_models[rowId];
	}
}
