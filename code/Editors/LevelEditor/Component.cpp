/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Component.h"

#include <assert.h>

namespace Editors
{
	Component::Component(const std::string& name)
		: m_name(name)
		, m_properties()
	{}

	Component::~Component()
	{
		for (const std::pair<std::string, Property*>& pair : m_properties)
			delete pair.second;

		m_properties.clear();
	}

	const std::string& Component::GetName() const
	{
		return m_name;
	}

	void Component::AddProperty(Property* property)
	{
		m_properties[property->GetName()] = property;
	}

	int Component::GetPropertyCount() const
	{
		return static_cast<int>(m_properties.size());
	}

	Property* Component::GetProperty(const std::string& name)
	{
		std::map<std::string, Property*>::iterator it = m_properties.find(name);
		if (it == m_properties.end())
			return nullptr;

		return it->second;
	}

	const Property* Component::GetProperty(const std::string& name) const
	{
		std::map<std::string, Property*>::const_iterator it = m_properties.find(name);
		if (it == m_properties.end())
			return nullptr;

		return it->second;
	}

	const Property* Component::GetProperty(int index) const
	{
		std::map<std::string, Property*>::const_iterator it = m_properties.begin();
		for (int ii = 0; ii < index; ++ii)
			++it;

		return it->second;
	}

	Property* Component::GetProperty(int index)
	{
		std::map<std::string, Property*>::const_iterator it = m_properties.begin();
		for (int ii = 0; ii < index; ++ii)
			++it;

		return it->second;
	}

	void Component::GetPropertyValue(const std::string& name, Systems::AssetId& id) const
	{
		const Property* pProperty = GetProperty(name);
		assert(pProperty);

		id = pProperty->GetValue<Systems::AssetId>();
	}

	void Component::GetPropertyValue(const std::string& name, Core::Mat44f& m) const
	{
		const Property* pProperty = GetProperty(name);
		assert(pProperty);

		m = pProperty->GetValue<Core::Mat44f>();
	}

	void Component::SetPropertyValue(const std::string& name, Systems::AssetId id)
	{
		Property* pProperty = GetProperty(name);
		assert(pProperty);

		switch (pProperty->GetType())
		{
		case PropertyType::kAssetMaterial:
			pProperty->SetMaterialAssetId(id);
			break;

		case PropertyType::kAssetMesh:
			pProperty->SetMeshAssetId(id);
			break;

		default:
			assert(false);
			break;

		}
	}

	void Component::SetPropertyValue(const std::string& name, const Core::Mat44f& m)
	{
		Property* pProperty = GetProperty(name);
		assert(pProperty);
		pProperty->SetMatrix(m);
	}
}
