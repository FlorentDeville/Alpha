/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

#include "Core/Callbacks/CallbackList.h"
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

	void InitializePropertyDescriptor();

	const char* PropertyTypeToString(PropertyType type);
	PropertyType StringToPropertyType(const char* str);

	class PropertyValue
	{
	public:
		PropertyValue();
		PropertyValue(const PropertyValue& other);
		~PropertyValue();

		bool operator==(const PropertyValue& other) const;

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
		Property(const Property& src);
		Property(const std::string& name, const Core::Mat44f& value);
		Property(const std::string& name, PropertyType type, Systems::AssetId value);

		~Property();

		const std::string& GetName() const;
		PropertyType GetType() const;

		template <class T> T& GetValue()
		{
			return m_value.GetValue<T>();
		}

		template <class T> const T& GetValue() const
		{
			return m_value.GetValue<T>();
		}

		void SetMatrix(const Core::Mat44f& value);
		void SetMeshAssetId(const Systems::AssetId& value);
		void SetMaterialAssetId(const Systems::AssetId& value);

		using OnValueChangedEvent = Core::CallbackList<void(const PropertyValue& oldValue, const PropertyValue& newValue)>;
		Core::CallbackId OnValueChanged(const OnValueChangedEvent::Callback& callback);
		void DisconnectOnValueChanged(const Core::CallbackId& id);

	private:
		std::string m_name;
		PropertyValue m_value;

		OnValueChangedEvent m_onValueChanged;
	};
}
