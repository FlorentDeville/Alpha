/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Sid/Sid.h"

#include "Systems/Objects/AssetObject.h"
#include "Systems/Objects/GameObject.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, LevelAsset_v1)
	class LevelAsset_v1 : public AssetObject
	{
	public:
		LevelAsset_v1();
		~LevelAsset_v1();

		static const std::string& GetAssetTypeName();
		static Core::Sid GetAssetTypeNameSid();

		void AddGameObject(GameObject* pGo);

		GameObject* FindGameObject(const Core::Guid& guid);

		const Core::Array<GameObject*>& GetGameObjectsArray() const;

	private:

		Core::Array<GameObject*> m_gameObjectsArray;

		START_REFLECTION(Systems::LevelAsset_v1)
			ADD_BASETYPE(Systems::Object)
			ADD_FIELD(m_gameObjectsArray)
		END_REFLECTION()
	};
}
