/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

#include "Core/Math/Mat44f.h"

#include "Systems/Assets/AssetId.h"

namespace Editors
{
	//Do not change the order. Always add at the end
	enum PropertyType
	{
		kUnknown = 0,
		kInt,
		kFloat,
		kString,
		kAssetMesh,			//AssetId
		kAssetMaterial,		//AssetId

		kVec4f,
		kMat44f,

		kCount
	};

	class PropertyValue
	{
	public:
		virtual ~PropertyValue(){}
	};

	class PropertyValueFloat : public PropertyValue
	{
	public:
		//PropertyValueFloat();
		PropertyValueFloat(float f)
			: m_f(f)
		{}

		//float Get() const;
		//void Set(float f);

	private:
		float m_f;
	};

	class PropertyValueMat44f : public PropertyValue
	{
	public:
		PropertyValueMat44f(const Core::Mat44f& m)
			: m_matrix(m)
		{}

		const Core::Mat44f& Get() const { return m_matrix; }
		Core::Mat44f& Get() { return m_matrix; }
		void Set(const Core::Mat44f& m) { m_matrix = m; }

	private:
		Core::Mat44f m_matrix;
	};

	class PropertyValueAssetId : public PropertyValue
	{
	public:
		PropertyValueAssetId(Systems::AssetId id)
			: m_id(id)
		{}

		Systems::AssetId Get() const { return m_id; }
		void Set(Systems::AssetId id) { m_id = id; }

	private:
		Systems::AssetId m_id;
	};

	// A property is a key value pair used to store and edit data.
	// It has basic reflection functionalities so it can be used by the editors.
	class Property
	{
	public:

		Property(const std::string& name, float value);
		Property(const std::string& name, const Core::Mat44f& value);
		Property(const std::string& name, PropertyType type, Systems::AssetId value);

		~Property();

		const std::string& GetName() const;
		PropertyType GetType() const;
		const PropertyValue& GetValue() const;
		PropertyValue& GetValue();

	private:
		std::string m_name;
		PropertyType m_type;
		PropertyValue* m_value;

	};
}
