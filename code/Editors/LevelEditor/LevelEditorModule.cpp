/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/LevelEditor/LevelEditorModule.h"

#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/SelectionMgr.h"
#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Rendering/Mesh/MeshMgr.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Game/InstanciateLevel.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Game/Subsystems/Camera/CameraSubsystem.h"
#include "Systems/Game/Subsystems/Clock/GameClockSubsystem.h"
#include "Systems/Objects/GameComponent.h"
#include "Systems/Objects/GameObject.h"
#include "Systems/Particle/ParticleSystem.h"

//#pragma optimize("", off)

namespace Editors
{
	LevelEditorModule::LevelEditorModule()
		: m_fovRad(DirectX::XMConvertToRadians(45.f))
		, m_pSelectionMgr(nullptr)
		, m_loadedLevelAssetId()
		, m_pLevel(nullptr)
		, m_pWorld(nullptr)
	{ }

	LevelEditorModule::~LevelEditorModule()
	{}

	void LevelEditorModule::Init()
	{
		m_pSelectionMgr = new SelectionMgr();

		m_pWorld = new Systems::GameContext();

		m_pWorld->m_pCameraSubsystem = new Systems::CameraSubsystem();
		m_pWorld->m_pParticleSystem = new Systems::ParticleSystem();
		m_pWorld->m_pClock = new Systems::GameClockSubsystem();
		m_pWorld->m_pClock->Start();
	}

	void LevelEditorModule::Shutdown()
	{
		delete m_pSelectionMgr;
		m_pSelectionMgr = nullptr;

		delete m_pWorld;
	}

	const SelectionMgr* LevelEditorModule::GetConstSelectionMgr() const
	{
		return m_pSelectionMgr;
	}

	SelectionMgr* LevelEditorModule::GetSelectionMgr()
	{
		return m_pSelectionMgr;
	}

	Systems::LevelAsset* LevelEditorModule::CreateNewLevel(const std::string& levelName)
	{
		Systems::LevelAsset* pLevel = Systems::AssetUtil::CreateAsset<Systems::LevelAsset>(levelName);
		if (!pLevel)
			return nullptr;

		const Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(pLevel->GetId());

		m_onNewLevel(*pMetadata);

		return pLevel;
	}

	void LevelEditorModule::DeleteLevel(Systems::NewAssetId id)
	{
		const Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(id);
		if (!pMetadata)
			return;

		//make a copy of the metadata to use it in the after delete event
		Systems::AssetMetadata metadataCopy = *pMetadata;

		m_onBeforeDeleteLevel(metadataCopy);

		Systems::AssetUtil::DeleteAsset(id);

		m_onAfterDeleteLevel(metadataCopy);
	}

	bool LevelEditorModule::SaveLevel()
	{
		if (!m_loadedLevelAssetId.IsValid())
			return false;

		bool res = Systems::ContainerMgr::Get().SaveContainer(m_loadedLevelAssetId.GetContainerId());
		if (!res)
			return res;

		m_onSaveLevel();
		return true;
	}

	bool LevelEditorModule::OpenLevel(Systems::NewAssetId id)
	{
		if (m_loadedLevelAssetId == id)
			return false;

		CloseLevel();

		Systems::LevelAsset* pLevel = Systems::AssetUtil::GetAsset<Systems::LevelAsset>(id, Systems::LoadingDomain::EDITOR);
		if (!pLevel)
		{
			pLevel = Systems::AssetUtil::LoadAsset<Systems::LevelAsset>(id, Systems::LoadingDomain::EDITOR);
			if (!pLevel)
				return false;
		}

		Systems::InstanciateLevel(pLevel, m_pWorld);

		m_loadedLevelAssetId = id;
		m_pLevel = pLevel;

		m_onOpenLevel();

		return true;
	}

	bool LevelEditorModule::CloseLevel()
	{
		if (!m_loadedLevelAssetId.IsValid())
			return false;

		ClearSelection();

		Systems::DeleteInstanciatedLevel(m_pLevel, m_pWorld);

		Systems::NewAssetId closedLevel = m_loadedLevelAssetId;
		m_loadedLevelAssetId = Systems::NewAssetId::INVALID;
		m_pLevel = nullptr;

		m_onClosedLevel(closedLevel);
		return true;
	}

	bool LevelEditorModule::RenameLevel(Systems::NewAssetId id, const std::string& newName)
	{
		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(id);
		if (!pMetadata)
			return false;

		pMetadata->SetVirtualName(newName);

		bool res = Systems::AssetMgr::Get().SaveMetadataTable();
		if (!res)
			return false;

		m_onRenameLevel(id, newName);

		return true;;
	}

	void LevelEditorModule::AddGameObject(const Core::Guid& parentGuid, Core::Guid& newGoGuid)
	{
		const Systems::GameObject* pGoParent = m_pLevel->FindGameObject(parentGuid);
		Systems::GameObject* pNewGo = nullptr;
		
		bool res = Internal_AddGameObject(pGoParent, &pNewGo);
		
		if (res)
		{
			newGoGuid = pNewGo->GetGuid();
			pNewGo->OnStart(m_pWorld);
			m_onAddGameObject(pNewGo, pGoParent);
		}
	}

	void LevelEditorModule::DeleteGameObject(const Core::Guid& guid)
	{
		if (!m_pLevel)
			return;

		m_pSelectionMgr->Remove(guid);
		
		m_onBeforeDeleteGameObject(guid);

		if (Systems::GameObject* pObject = m_pLevel->FindGameObject(guid))
			pObject->OnDestroy(m_pWorld);

		m_pLevel->DeleteGameObject(guid);

		m_onAfterDeleteGameObject(guid);
	}

	void LevelEditorModule::RenameGameObject(const Core::Guid& guid, const std::string& name)
	{
		if (!m_pLevel)
			return;

		Systems::GameObject* pGo = m_pLevel->FindGameObject(guid);
		if (!pGo)
			return;

		const std::vector<Core::FieldDescriptor*>& fields = pGo->GetTypeDescriptor()->GetFields();
		std::vector<Core::FieldDescriptor*>::const_iterator it = std::find_if(fields.begin(), fields.end(), 
			[](const Core::FieldDescriptor* pField) { return pField->GetName() == "m_name"; });

		if (it == fields.cend())
			return;

		const Core::FieldDescriptor* pField = *it;
		const void* pValue = reinterpret_cast<const void*>(&name);
		ObjectWatcher::Get().ModifyField(pGo, pField, ObjectWatcher::SET_FIELD, 0, pValue);
	}

	void LevelEditorModule::ReparentGameObject(const Core::Guid& parent, const Core::Guid& child)
	{
		//first check all the inputs are valid
		if (!m_pLevel)
			return;

		Systems::GameObject* pGoChild = m_pLevel->FindGameObject(child);
		if (!pGoChild)
			return;

		Systems::TransformComponent& childTx = pGoChild->GetTransform();
		Core::Guid oldParentGuid = childTx.GetParentGuid();
		Systems::GameObject* pGoOldParent = m_pLevel->FindGameObject(oldParentGuid);

		Systems::GameObject* pGoNewParent = nullptr;
		if (parent.IsValid())
		{
			pGoNewParent = m_pLevel->FindGameObject(parent);
			if (!pGoNewParent)
				return;
		}

		//if the new parent and old parent are the same then there is nothing to do
		if (pGoNewParent == pGoOldParent)
			return;

		m_pLevel->Reparent(pGoChild, pGoNewParent);

		m_onReparentGameObject(pGoChild, pGoOldParent, pGoNewParent);
	}

	void LevelEditorModule::DuplicateGameObject(const Core::Guid& nodeGuid, Core::Guid& newNodeGuid)
	{
		const Systems::GameObject* pSrcObj = m_pLevel->FindGameObject(nodeGuid);
		if (!pSrcObj)
			return;

		const Systems::GameObject* pParent = pSrcObj->GetTransform().GetParent();
		Systems::GameObject* pNewGo = nullptr;
		bool res = Internal_AddGameObject(pParent, &pNewGo);
		if (!res)
			return;

		newNodeGuid = pNewGo->GetGuid();

		//set new name
		{
			uint32_t newSuffix = m_pLevel->GetGameObjectsArray().GetSize();
			const int BUFFER_SIZE = 32;
			char buffer[BUFFER_SIZE] = { '\0' };

			//first find the base name
			size_t pos = pSrcObj->GetName().find_last_of('_');
			if (pos == std::string::npos)
			{
				snprintf(buffer, BUFFER_SIZE, "%s_%d", pSrcObj->GetName().c_str(), newSuffix);
			}
			else
			{
				bool isInteger = true;
				std::string end = pSrcObj->GetName().substr(pos+1);
				for (char c : end)
				{
					if (!std::isdigit(c))
					{
						isInteger = false;
						break;
					}
				}

				if (isInteger)
				{
					std::string baseName = pSrcObj->GetName().substr(0, pos);
					snprintf(buffer, BUFFER_SIZE, "%s_%d", baseName.c_str(), newSuffix);

				}
				else
				{
					snprintf(buffer, BUFFER_SIZE, "%s_%d", pSrcObj->GetName().c_str(), newSuffix);
				}
			}
		
			pNewGo->SetName(buffer);
		}

		pNewGo->GetTransform().SetLocalTx(pSrcObj->GetTransform().GetLocalTx());
		pNewGo->GetTransform().ComputeWorldTx();

		const Core::Array<Systems::GameComponent*>& components = pSrcObj->GetComponents();
		for (const Systems::GameComponent* pSrcComponent : components)
		{
			const Core::TypeDescriptor* pSrcComponentType = pSrcComponent->GetTypeDescriptor();
			Systems::GameComponent* pDstComponent = Systems::CreateNewGameComponent(pSrcComponentType);
			pSrcComponentType->Copy(pSrcComponent, pDstComponent);
			pNewGo->AddComponent(pDstComponent);
		}

		pNewGo->OnStart(m_pWorld);

		m_onAddGameObject(pNewGo, pParent);
	}

	void LevelEditorModule::SetCameraWs(const Core::Mat44f& ws)
	{
		m_cameraWs = ws;
	}

	const Core::Mat44f& LevelEditorModule::GetCameraWs() const
	{
		return m_cameraWs;
	}

	float LevelEditorModule::GetFovRad() const
	{
		return m_fovRad;
	}

	bool LevelEditorModule::IsSelected(const Core::Guid& nodeGuid) const
	{
		return m_pSelectionMgr->IsSelected(nodeGuid);
	}

	bool LevelEditorModule::AddToSelection(const Core::Guid& nodeGuid)
	{
		return m_pSelectionMgr->Add(nodeGuid);
	}

	bool LevelEditorModule::RemoveFromSelection(const Core::Guid& nodeGuid)
	{
		return m_pSelectionMgr->Remove(nodeGuid);
	}

	void LevelEditorModule::ClearSelection()
	{
		m_pSelectionMgr->Clear();
	}

	Systems::NewAssetId LevelEditorModule::GetCurrentLoadedLevelAssetId() const
	{
		return m_loadedLevelAssetId;
	}

	std::string LevelEditorModule::GetCurrentLoadedLevelName() const
	{
		if (!m_loadedLevelAssetId.IsValid())
			return "";

		const Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(m_loadedLevelAssetId);
		if (!pMetadata)
			return "";

		return pMetadata->GetVirtualName();
	}

	const Systems::LevelAsset* LevelEditorModule::GetCurrentLoadedLevel() const
	{
		return m_pLevel;
	}

	Systems::LevelAsset* LevelEditorModule::GetCurrentLoadedLevel()
	{
		return m_pLevel;
	}

	const Systems::GameContext* LevelEditorModule::GetWorld() const
	{
		return m_pWorld;
	}

	Systems::GameContext* LevelEditorModule::GetWorld()
	{
		return m_pWorld;
	}

	bool LevelEditorModule::Internal_AddGameObject(const Systems::GameObject* pParent, Systems::GameObject** ppObj)
	{
		if (!m_pLevel)
			return false;

		*ppObj = Systems::CreateNewGameObject<Systems::GameObject>("newentity");

		Core::Mat44f localTx;
		localTx.SetIdentity();
		(*ppObj)->GetTransform().SetLocalTx(localTx);
		(*ppObj)->GetTransform().ComputeWorldTx();

		Core::Guid parentGuid;
		if (pParent)
			parentGuid = pParent->GetGuid();

		m_pLevel->AddGameObject(*ppObj, parentGuid);

		return true;
	}
}
