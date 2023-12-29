/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

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

	union PropertyValue
	{
		int32_t m_int;
		float m_float;
		std::string m_string;
		Systems::AssetId m_assetId;
		Core::Vec4f m_vec4f;
		Core::Mat44f m_mat44f;

		PropertyValue();
		~PropertyValue();
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

		template <class T> T& GetValue()
		{
			T& castedValue = reinterpret_cast<T&>(m_value);
			return castedValue;
		}

		template <class T> const T& GetValue() const
		{
			const T& castedValue = reinterpret_cast<const T&>(m_value);
			return castedValue;
		}

		void SetMeshAssetId(const Systems::AssetId& id);
		void SetMaterialAssetId(const Systems::AssetId& id);
		void SetValue(const Core::Mat44f& matrix);

	private:
		std::string m_name;
		PropertyType m_type;
		PropertyValue m_value;
	};
}
