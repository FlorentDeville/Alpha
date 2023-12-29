/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Property.h"

namespace Editors
{
	PropertyValue::PropertyValue()
	{}

	PropertyValue::~PropertyValue()
	{}

	Property::Property(const std::string& name, float value)
		: m_name(name)
		, m_type(kFloat)
	{
		m_value.m_float = value;
	}

	Property::Property(const std::string& name, const Core::Mat44f& value)
		: m_name(name)
		, m_type(kMat44f)
	{
		m_value.m_mat44f = value;
	}

	Property::Property(const std::string& name, PropertyType type, Systems::AssetId value)
		: m_name(name)
		, m_type(type)
	{
		m_value.m_assetId = value;
	}

	Property::~Property()
	{}

	const std::string& Property::GetName() const
	{
		return m_name;
	}

	PropertyType Property::GetType() const
	{
		return m_type;
	}

	void Property::SetMeshAssetId(const Systems::AssetId& id)
	{
		m_value.m_assetId = id;
		m_type = PropertyType::kAssetMesh;
	}

	void Property::SetMaterialAssetId(const Systems::AssetId& id)
	{
		m_value.m_assetId = id;
		m_type = PropertyType::kAssetMaterial;
	}

	void Property::SetValue(const Core::Mat44f& matrix)
	{
		m_value.m_mat44f = matrix;
	}
}
