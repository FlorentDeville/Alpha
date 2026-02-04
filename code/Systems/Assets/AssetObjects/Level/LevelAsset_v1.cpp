/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetObjects/Level/LevelAsset_v1.h"

#include "Systems/Objects/GameObject.h"

namespace Systems
{
	LevelAsset_v1::LevelAsset_v1()
		: AssetObject()
		, m_gameObjectsArray()
		, m_roots()
		, m_rootsGameObject()
	{ }

	LevelAsset_v1::~LevelAsset_v1()
	{
		for (GameObject* pGameObject : m_gameObjectsArray)
			delete pGameObject;

		m_gameObjectsArray.Clear();
		m_roots.Clear();
		m_rootsGameObject.Clear();
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

		m_roots.PushBack(pGo->GetGuid());
		m_rootsGameObject.PushBack(pGo);
	}

	void LevelAsset_v1::AddGameObject(GameObject* pGo, const Core::Guid& parent)
	{
		if (!parent.IsValid())
		{
			AddGameObject(pGo);
			return;
		}

		m_gameObjectsArray.PushBack(pGo);

		GameObject* pGoParent = FindGameObject(parent);
		if (!pGoParent)
			return;

		pGoParent->GetTransform().AddChild(pGo->GetGuid());
		pGo->GetTransform().SetParent(pGoParent);
	}

	void LevelAsset_v1::DeleteGameObject(GameObject* pGo)
	{
		const Core::Guid& parentGuid = pGo->GetTransform().GetParentGuid();
		if (GameObject* pGoParent = FindGameObject(parentGuid))
		{
			pGoParent->GetTransform().RemoveChild(pGo->GetGuid());
		}
		else
		{
			m_roots.Erase(pGo->GetGuid());
			m_rootsGameObject.Erase(pGo);
		}

		m_gameObjectsArray.Erase(pGo);
		Systems::DeleteObject(pGo);
	}

	void LevelAsset_v1::DeleteGameObject(const Core::Guid& guid)
	{
		GameObject* pGo = FindGameObject(guid);
		if (!pGo)
			return;

		DeleteGameObject(pGo);
	}

	GameObject* LevelAsset_v1::FindGameObject(const Core::Guid& guid)
	{
		Core::Array<GameObject*>::Iterator it = std::find_if(m_gameObjectsArray.cbegin(), m_gameObjectsArray.cend(), [guid](const GameObject* pGo) { return pGo->GetGuid() == guid; });
		if (it == m_gameObjectsArray.cend())
			return nullptr;

		return *it;
	}

	const GameObject* LevelAsset_v1::FindGameObject(const Core::Guid& guid) const
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

	void LevelAsset_v1::PostLoad()
	{
		for (const Core::Guid& root : m_roots)
		{
			if (GameObject* pRootGo = FindGameObject(root))
				m_rootsGameObject.PushBack(pRootGo);
		}

		for (Systems::GameObject* pGo : m_gameObjectsArray)
		{
			const Core::Guid& parentGuid = pGo->GetTransform().GetParentGuid();
			if (!parentGuid.IsValid())
				continue;

			GameObject* pParentGo = FindGameObject(parentGuid);
			if (!pParentGo)
				continue;

			pGo->GetTransform().SetParent(pParentGo);
		}
	}
}
