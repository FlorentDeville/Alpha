/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Property.h"

namespace Editors
{
	struct PropertyTypeDescriptor
	{
		PropertyType m_type;
		std::string m_strType;
	};

	std::map<PropertyType, PropertyTypeDescriptor> s_propertyTypeDescriptors;

	void InitializePropertyDescriptor()
	{
		s_propertyTypeDescriptors[kInt] =			{ kInt,				"int" };
		s_propertyTypeDescriptors[kFloat] =			{ kFloat,			"float" };
		s_propertyTypeDescriptors[kString] =		{ kString,			"string" };
		s_propertyTypeDescriptors[kAssetMesh] =		{ kAssetMesh,		"mesh" };
		s_propertyTypeDescriptors[kAssetMaterial] = { kAssetMaterial,	"material" };
		s_propertyTypeDescriptors[kVec4f] =			{ kVec4f,			"vec4f" };
		s_propertyTypeDescriptors[kMat44f] =		{ kMat44f,			"mat44f" };
	}

	const char* PropertyTypeToString(PropertyType type)
	{
		return s_propertyTypeDescriptors[type].m_strType.c_str();
	}

	PropertyType StringToPropertyType(const char* str)
	{
		for (const std::pair<PropertyType, PropertyTypeDescriptor>& pair : s_propertyTypeDescriptors)
		{
			if (strcmp(str, pair.second.m_strType.c_str()) == 0)
				return pair.first;
		}

		return kUnknown;
	}

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

	Property::Property(const std::string& name)
		: m_name(name)
	{}

	Property::Property(const Property& src)
		: m_name(src.m_name)
		, m_value(src.m_value)
	{}

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
