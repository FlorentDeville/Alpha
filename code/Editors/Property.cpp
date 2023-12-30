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

	PropertyValue::PropertyValue(const PropertyValue& other)
		: m_type(other.GetType())
	{
		if (other.GetType() == kString)
			m_internalValue.m_string = other.GetValue<std::string>();
		else
			memcpy(&m_internalValue, &other.m_internalValue, sizeof(InternalValue));
	}

	PropertyValue::~PropertyValue()
	{}

	bool PropertyValue::operator==(const PropertyValue& other) const
	{
		if (m_type != other.GetType())
			return false;

		if (m_type == kString)
			return m_internalValue.m_string == other.GetValue<std::string>();

		return memcmp(&m_internalValue, &other.m_internalValue, sizeof(InternalValue)) == 0;
	}

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

	PropertyType Property::GetType() const
	{
		return m_value.GetType();
	}

	void Property::SetMatrix(const Core::Mat44f& value)
	{
		PropertyValue oldValue = m_value;
		m_value.SetMatrix(value);

		if (m_value == oldValue)
			return;

		if (m_onValueChanged)
			m_onValueChanged(oldValue, m_value);
	}

	void Property::SetMeshAssetId(const Systems::AssetId& value)
	{
		PropertyValue oldValue = m_value;
		m_value.SetMeshAssetId(value);

		if (m_value == oldValue)
			return;

		if (m_onValueChanged)
			m_onValueChanged(oldValue, m_value);
	}

	void Property::SetMaterialAssetId(const Systems::AssetId& value)
	{
		PropertyValue oldValue = m_value;
		m_value.SetMaterialAssetId(value);

		if (m_value == oldValue)
			return;

		if (m_onValueChanged)
			m_onValueChanged(oldValue, m_value);
	}

	Core::CallbackId Property::OnValueChanged(const OnValueChangedEvent::Callback& callback)
	{
		return m_onValueChanged.Connect(callback);
	}

	void Property::DisconnectOnValueChanged(const Core::CallbackId& id)
	{
		m_onValueChanged.Disconnect(id);
	}
}
