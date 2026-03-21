/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/Level/LevelAsset.h"

#include "Systems/Objects/GameObject.h"

namespace Systems
{
	LevelAsset::LevelAsset()
		: AssetObject()
		, m_gameObjectsArray()
		, m_roots()
		, m_rootsGameObject()
	{ }

	LevelAsset::~LevelAsset()
	{
		for (GameObject* pGameObject : m_gameObjectsArray)
			delete pGameObject;

		m_gameObjectsArray.Clear();
		m_roots.Clear();
		m_rootsGameObject.Clear();
	}

	const std::string& LevelAsset::GetAssetTypeName()
	{
		static std::string name = "Level";
		return name;
	}

	Core::Sid LevelAsset::GetAssetTypeNameSid()
	{
		static Core::Sid sid = SID(GetAssetTypeName());
		return sid;
	}

	void LevelAsset::AddGameObject(GameObject* pGo)
	{
		m_gameObjectsArray.PushBack(pGo);

		m_roots.PushBack(pGo->GetGuid());
		m_rootsGameObject.PushBack(pGo);
	}

	void LevelAsset::AddGameObject(GameObject* pGo, const Core::Guid& parent)
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

		pGoParent->GetTransform().AddChild(pGo);
		pGo->GetTransform().SetParent(pGoParent);
	}

	void LevelAsset::Reparent(GameObject* pGo, GameObject* pGoNewParent)
	{
		//add the child to its new parent if any
		if (pGoNewParent)
		{
			pGoNewParent->GetTransform().AddChild(pGo);

			//compute the new local to keep the object in the same world transform
			Core::Mat44f txPs = pGoNewParent->GetTransform().GetWorldTx();
			Core::Mat44f txInvPs = txPs.FastInverse();
			Core::Mat44f newTxLs = pGo->GetTransform().GetWorldTx() * txInvPs;
			pGo->GetTransform().SetLocalTx(newTxLs);
		}
		else
		{
			//no new parent then I'm a root object
			m_roots.PushBack(pGo->GetGuid());
			m_rootsGameObject.PushBack(pGo);

			pGo->GetTransform().SetLocalTx(pGo->GetTransform().GetWorldTx());
		}

		//remove the child from its old parent
		Systems::GameObject* pGoOldParent = FindGameObject(pGo->GetTransform().GetParentGuid());
		if (pGoOldParent)
		{
			Systems::TransformComponent& oldParentTx = pGoOldParent->GetTransform();
			oldParentTx.RemoveChild(pGo->GetGuid());
		}
		else
		{
			//if I had no parent, I was a root object. remove from the list of root
			m_roots.Erase(pGo->GetGuid());
			m_rootsGameObject.Erase(pGo);
		}

		//set the new parent to the child
		pGo->GetTransform().SetParent(pGoNewParent);
		pGo->GetTransform().Update();
	}

	void LevelAsset::DeleteGameObject(GameObject* pGo)
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

		DeleteChildrenRecursive(pGo);

		m_gameObjectsArray.Erase(pGo);
		Systems::DeleteObject(pGo);
	}

	void LevelAsset::DeleteGameObject(const Core::Guid& guid)
	{
		GameObject* pGo = FindGameObject(guid);
		if (!pGo)
			return;

		DeleteGameObject(pGo);
	}

	GameObject* LevelAsset::FindGameObject(const Core::Guid& guid)
	{
		Core::Array<GameObject*>::Iterator it = std::find_if(m_gameObjectsArray.cbegin(), m_gameObjectsArray.cend(), [guid](const GameObject* pGo) { return pGo->GetGuid() == guid; });
		if (it == m_gameObjectsArray.cend())
			return nullptr;

		return *it;
	}

	const GameObject* LevelAsset::FindGameObject(const Core::Guid& guid) const
	{
		Core::Array<GameObject*>::Iterator it = std::find_if(m_gameObjectsArray.cbegin(), m_gameObjectsArray.cend(), [guid](const GameObject* pGo) { return pGo->GetGuid() == guid; });
		if (it == m_gameObjectsArray.cend())
			return nullptr;

		return *it;
	}

	const Core::Array<GameObject*>& LevelAsset::GetGameObjectsArray() const
	{
		return m_gameObjectsArray;
	}

	Core::Array<GameObject*>& LevelAsset::GetRootGameObjects()
	{
		return m_rootsGameObject;
	}

	void LevelAsset::PostLoad()
	{
		for (const Core::Guid& root : m_roots)
		{
			if (GameObject* pRootGo = FindGameObject(root))
				m_rootsGameObject.PushBack(pRootGo);
		}

		for (Systems::GameObject* pGo : m_gameObjectsArray)
		{
			const Core::Guid& parentGuid = pGo->GetTransform().GetParentGuid();
			if (parentGuid.IsValid())
			{
				if(GameObject* pParentGo = FindGameObject(parentGuid))
					pGo->GetTransform().SetParent(pParentGo);
			}

			const Core::Array<Core::Guid>& children = pGo->GetTransform().GetChildrenGuid();
			for (const Core::Guid& childGuid : children)
			{
				GameObject* pChildGo = FindGameObject(childGuid);
				if (!pChildGo)
					continue;

				pGo->GetTransform().AddChildCachedPointer(pChildGo);
			}
		}
	}

	void LevelAsset::DeleteChildrenRecursive(GameObject* pGo)
	{
		TransformComponent& transform = pGo->GetTransform();
		for (Systems::GameObject* pChild : transform.m_childrenGo)
		{
			DeleteChildrenRecursive(pChild);

			m_gameObjectsArray.Erase(pChild);
			Systems::DeleteObject(pChild);
		}
	}
}
