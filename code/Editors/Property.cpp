/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Property.h"

namespace Editors
{
	PropertyValue::InternalValue::InternalValue()
	{}

	PropertyValue::InternalValue::~InternalValue()
	{}

	PropertyValue::PropertyValue()
		: m_type(kUnknown)
	{}

	PropertyValue::~PropertyValue()
	{}

	PropertyType PropertyValue::GetType() const
	{
		return m_type;
	}

	void PropertyValue::SetMatrix(const Core::Mat44f& value)
	{
		m_type = kMat44f;
		m_internalValue.m_mat44f = value;
	}

	void PropertyValue::SetMeshAssetId(const Systems::AssetId& value)
	{
		m_type = kAssetMesh;
		m_internalValue.m_assetId = value;
	}

	void PropertyValue::SetMaterialAssetId(const Systems::AssetId& value)
	{
		m_type = kAssetMaterial;
		m_internalValue.m_assetId = value;
	}

	Property::Property(const std::string& name, const Core::Mat44f& value)
		: m_name(name)
	{
		m_value.SetMatrix(value);
	}

	Property::Property(const std::string& name, PropertyType type, Systems::AssetId value)
		: m_name(name)
	{
		if (type == PropertyType::kAssetMaterial)
			m_value.SetMaterialAssetId(value);
		else if (type == PropertyType::kAssetMesh)
			m_value.SetMeshAssetId(value);
	}

	Property::~Property()
	{}

	const std::string& Property::GetName() const
	{
		return m_name;
	}

	PropertyValue& Property::GetValue()
	{
		return m_value;
	}

	const PropertyValue& Property::GetConstValue() const
	{
		return m_value;
	}
}
