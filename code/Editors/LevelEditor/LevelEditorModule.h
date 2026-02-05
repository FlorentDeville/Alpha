/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Core/Callbacks/CallbackList.h"
#include "Core/Callbacks/CallbackMacro.h"
#include "Core/Math/Mat44f.h"

#include "Systems/Assets/AssetId.h"
#include "Systems/Assets/AssetObjects/Level/LevelAsset.h"

#include <map>
#include <string>

namespace Core
{
	class Guid;
}

namespace Rendering
{
	class MaterialId;
	class MeshId;
}

namespace Systems
{
	class AssetId;
	class AssetMetadata;
}

namespace Widgets
{
	class Widget;
}

namespace Editors
{
	class Node;
	class GizmoWidget;
	class SelectionMgr;

	class LevelEditorModule : public Core::Singleton<LevelEditorModule>
	{
	public:
		LevelEditorModule();
		~LevelEditorModule();

		void Init() override;
		void Shutdown() override;

		const SelectionMgr* GetConstSelectionMgr() const;
		SelectionMgr* GetSelectionMgr();

		Systems::LevelAsset* CreateNewLevel(const std::string& levelName);
		void DeleteLevel(Systems::NewAssetId id);

		//Save the currently opened level.
		bool SaveLevel();

		bool OpenLevel(Systems::NewAssetId id);

		bool RenameLevel(Systems::NewAssetId id, const std::string& newName);

		void AddGameObject(const Core::Guid& parentGuid, Core::Guid& newGoGuid);
		void DeleteGameObject(const Core::Guid & nodeGuid);
		void RenameGameObject(const Core::Guid& guid, const std::string& name);

		void ReparentGameObject(const Core::Guid& parent, const Core::Guid& child);

		void SetCameraWs(const Core::Mat44f& ws);
		const Core::Mat44f& GetCameraWs() const;

		float GetFovRad() const;

		Rendering::MeshId LoadMesh(Systems::AssetId id);
		Rendering::MaterialId LoadMaterial(Systems::AssetId id);

		bool AddToSelection(const Core::Guid& nodeGuid);
		bool RemoveFromSelection(const Core::Guid& nodeGuid);
		void ClearSelection();

		Systems::NewAssetId GetCurrentLoadedLevelAssetId() const;
		std::string GetCurrentLoadedLevelName() const;

		const Systems::LevelAsset* GetCurrentLoadedLevel() const;
		Systems::LevelAsset* GetCurrentLoadedLevel();

		//operation callback
		EVENT_DECL(NewLevel, void(const Systems::AssetMetadata& metadata))
		EVENT_DECL(BeforeDeleteLevel, void(Systems::AssetMetadata& metadata))
		EVENT_DECL(AfterDeleteLevel, void(Systems::AssetMetadata& metadata))

		EVENT_DECL(OpenLevel, void())
		EVENT_DECL(SaveLevel, void())
		EVENT_DECL(RenameLevel, void(Systems::NewAssetId id, const std::string& newName))

		EVENT_DECL(AddGameObject, void(const Systems::GameObject* pGo, const Systems::GameObject* pGoParent))
		EVENT_DECL(BeforeDeleteGameObject, void(const Core::Guid& nodeGuid))
		EVENT_DECL(AfterDeleteGameObject, void(const Core::Guid& nodeGuid))
		EVENT_DECL(DuplicateEntity, void(const Core::Guid& src, const Core::Guid& copy))
		EVENT_DECL(ReparentGameObject, void(const Systems::GameObject* pGo, const Systems::GameObject* pOldParent, const Systems::GameObject* pNewParent))

		//temp
		std::map<Systems::AssetId, Rendering::MeshId> m_assetIdToMeshId;
		std::map<Systems::AssetId, Rendering::MaterialId> m_assetIdToMaterialId;

	private:
		SelectionMgr* m_pSelectionMgr;

		Systems::NewAssetId m_loadedLevelAssetId;
		Systems::LevelAsset* m_pLevel;

		//camera state
		Core::Mat44f m_cameraWs;
		float m_fovRad;
	};
}
