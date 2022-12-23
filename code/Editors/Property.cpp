/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Property.h"

namespace Editors
{
	Property::Property(const std::string& name, float value)
		: m_name(name)
		, m_type(kFloat)
		, m_value(new PropertyValueFloat(value))
	{}

	Property::Property(const std::string& name, const Core::Mat44f& value)
		: m_name(name)
		, m_type(kMat44f)
		, m_value(new PropertyValueMat44f(value))
	{}

	Property::Property(const std::string& name, PropertyType type, Systems::AssetId value)
		: m_name(name)
		, m_type(type)
		, m_value(new PropertyValueAssetId(value))
	{}

	Property::~Property()
	{
		delete m_value;
	}

	const std::string& Property::GetName() const
	{
		return m_name;
	}

	PropertyType Property::GetType() const
	{
		return m_type;
	}

	const PropertyValue& Property::GetValue() const
	{
		return *m_value;
	}

	PropertyValue& Property::GetValue()
	{
		return *m_value;
	}
}
