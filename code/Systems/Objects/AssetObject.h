/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Objects/Object.h"

#include "Systems/Assets/NewAssetId.h"
#include "Systems/Reflection/ReflectionMacro.h"

namespace Systems
{
	//Base class for any asset
	ENABLE_REFLECTION_WITH_NS(Systems, AssetObject)
	class AssetObject : public Object
	{
	public:
		AssetObject() = default;
		AssetObject(NewAssetId id);

		~AssetObject() = default;

		void SetAssetId(NewAssetId id);

		NewAssetId GetId() const;

	private:
		NewAssetId m_id;

		START_REFLECTION(Systems::AssetObject)
			ADD_BASETYPE(Systems::Object)
			ADD_FIELD(m_id)
		END_REFLECTION()
	};

	//Create a new asset with a new asset id
	template<typename T, typename... Args> T* CreateNewAsset(Args... args)
	{
		T* pAsset = CreateObject<T>(std::forward<Args>(args)...);
		return pAsset;
	}
}
