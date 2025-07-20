/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetObjects/Level/LevelAsset_v1.h"

namespace Systems
{
	LevelAsset_v1::LevelAsset_v1()
		: AssetObject()
		, m_gameObjectsArray()
	{ }

	LevelAsset_v1::~LevelAsset_v1()
	{
		for (GameObject* pGameObject : m_gameObjectsArray)
			delete pGameObject;

		m_gameObjectsArray.Clear();
	}

	const std::string& LevelAsset_v1::GetAssetTypeName()
	{
		static std::string name = "Level";
		return name;
	}

	Core::Sid LevelAsset_v1::GetAssetTypeNameSid()
	{
		static Core::Sid sid = SID(GetAssetTypeName());
		return sid;
	}

	void LevelAsset_v1::AddGameObject(GameObject* pGo)
	{
		m_gameObjectsArray.PushBack(pGo);
	}

	GameObject* LevelAsset_v1::FindGameObject(const Core::Guid& guid)
	{
		Core::Array<GameObject*>::Iterator it = std::find_if(m_gameObjectsArray.cbegin(), m_gameObjectsArray.cend(), [guid](const GameObject* pGo) { return pGo->GetGuid() == guid; });
		if (it == m_gameObjectsArray.cend())
			return nullptr;

		return *it;
	}

	const Core::Array<GameObject*>& LevelAsset_v1::GetGameObjectsArray() const
	{
		return m_gameObjectsArray;
	}
}
