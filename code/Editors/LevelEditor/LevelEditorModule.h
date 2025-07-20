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
	class LevelMgr;
	class SelectionMgr;

	class LevelEditorModule : public Core::Singleton<LevelEditorModule>
	{
	public:
		LevelEditorModule();
		~LevelEditorModule();

		void Init() override;
		void Shutdown() override;

		const LevelMgr* GetConstLevelMgr() const;
		LevelMgr* GetLevelMgr();

		const SelectionMgr* GetConstSelectionMgr() const;
		SelectionMgr* GetSelectionMgr();

		Systems::LevelAsset* CreateNewLevel(const std::string& levelName);
		void DeleteLevel(Systems::NewAssetId id);

		//Save the currently opened level.
		bool SaveLevel();

		bool OpenLevel(Systems::NewAssetId id);

		bool RenameLevel(Systems::NewAssetId id, const std::string& newName);

		void AddGameObject(Core::Guid& nodeGuid);
		void DeleteGameObject(const Core::Guid & nodeGuid);
		void RenameGameObject(const Core::Guid& guid, const std::string& name);
		void DuplicateEntity(const Core::Guid& originalNode, Core::Guid& newNode);

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

		//operation callback
		EVENT_DECL(NewLevel, void(const Systems::AssetMetadata& metadata))
		EVENT_DECL(BeforeDeleteLevel, void(Systems::AssetMetadata& metadata))
		EVENT_DECL(AfterDeleteLevel, void(Systems::AssetMetadata& metadata))

		EVENT_DECL(OpenLevel, void())
		EVENT_DECL(SaveLevel, void())
		EVENT_DECL(RenameLevel, void(Systems::NewAssetId id, const std::string& newName))

		EVENT_DECL(AddGameObject, void(const Systems::GameObject* pGo))
		EVENT_DECL(BeforeDeleteGameObject, void(const Core::Guid& nodeGuid))
		EVENT_DECL(AfterDeleteGameObject, void(const Core::Guid& nodeGuid))
		EVENT_DECL(RenameGameObject, void(const Core::Guid & guid, const std::string & newName))
		EVENT_DECL(DuplicateEntity, void(const Core::Guid& src, const Core::Guid& copy))

		//temp
		std::map<Systems::AssetId, Rendering::MeshId> m_assetIdToMeshId;
		std::map<Systems::AssetId, Rendering::MaterialId> m_assetIdToMaterialId;

	private:
		LevelMgr* m_pLevelMgr;
		SelectionMgr* m_pSelectionMgr;

		Systems::NewAssetId m_loadedLevelAssetId;
		Systems::LevelAsset* m_pLevel;

		//camera state
		Core::Mat44f m_cameraWs;
		float m_fovRad;
	};
}
