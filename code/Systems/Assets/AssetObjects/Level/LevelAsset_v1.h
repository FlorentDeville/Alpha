/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Guid/Guid.h"
#include "Core/Sid/Sid.h"

#include "Systems/Objects/AssetObject.h"
#include "Systems/Assets/AssetObjects/Level/LevelAsset.h"

namespace Core
{
	class Guid;
}

ENABLE_REFLECTION(Systems, LevelAsset_v1)

namespace Systems
{
	class GameObject;

	class LevelAsset_v1 : public AssetObject
	{
		friend class LevelAsset;
	public:
		LevelAsset_v1();
		~LevelAsset_v1();

		static const std::string& GetAssetTypeName();
		static Core::Sid GetAssetTypeNameSid();

		// Add game object to the root of the scene
		void AddGameObject(GameObject* pGo);

		// Add new game object to the specific parent. Invalid guid add it to the root
		void AddGameObject(GameObject* pGo, const Core::Guid& parent);

		void DeleteGameObject(GameObject* pGo);
		void DeleteGameObject(const Core::Guid& guid);

		GameObject* FindGameObject(const Core::Guid& guid);
		const GameObject* FindGameObject(const Core::Guid& guid) const;

		const Core::Array<GameObject*>& GetGameObjectsArray() const;

		Core::Array<GameObject*>& GetRootGameObjects();

		void PostLoad() override;

	private:

		Core::Array<GameObject*> m_gameObjectsArray;
		Core::Array<Core::Guid> m_roots;

		START_REFLECTION(Systems::LevelAsset_v1)
			ADD_BASETYPE(Systems::Object)
			ADD_FIELD(m_gameObjectsArray)
			ADD_FIELD(m_roots)
			ADD_AUTO_UPGRADE(Systems::LevelAsset)
		END_REFLECTION()

		//Cached pointers to the root game objects
		Core::Array<GameObject*> m_rootsGameObject;
	};
}
