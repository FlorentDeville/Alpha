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

	class PropertyValue
	{
	public:
		PropertyValue();
		~PropertyValue();

		PropertyType GetType() const;

		template <class T> T& GetValue()
		{
			T& castedValue = reinterpret_cast<T&>(m_internalValue);
			return castedValue;
		}

		template <class T> const T& GetValue() const
		{
			const T& castedValue = reinterpret_cast<const T&>(m_internalValue);
			return castedValue;
		}

		void SetMatrix(const Core::Mat44f& value);
		void SetMeshAssetId(const Systems::AssetId& value);
		void SetMaterialAssetId(const Systems::AssetId& value);

	private:
		union InternalValue
		{
			int32_t m_int;
			float m_float;
			std::string m_string;
			Systems::AssetId m_assetId;
			Core::Vec4f m_vec4f;
			Core::Mat44f m_mat44f;

			InternalValue();
			~InternalValue();
		};

		InternalValue m_internalValue;
		PropertyType m_type;
	};

	// A property is a key value pair used to store and edit data.
	// It has basic reflection functionalities so it can be used by the editors.
	class Property
	{
	public:
		Property(const std::string& name, const Core::Mat44f& value);
		Property(const std::string& name, PropertyType type, Systems::AssetId value);

		~Property();

		const std::string& GetName() const;
		PropertyValue& GetValue();
		const PropertyValue& GetConstValue() const;

	private:
		std::string m_name;
		PropertyValue m_value;
	};
}
