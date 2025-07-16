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

namespace Os
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
		bool SaveLevel();
		bool SaveAsLevel(Systems::AssetId levelId);
		bool LoadLevel(Systems::AssetId levelId);

		void AddNewEntity(Os::Guid& nodeGuid);
		void DeleteEntity(const Os::Guid& nodeGuid);
		void RenameEntity(const Os::Guid& nodeGuid, const std::string& name);
		void DuplicateEntity(const Os::Guid& originalNode, Os::Guid& newNode);

		void SetCameraWs(const Core::Mat44f& ws);
		const Core::Mat44f& GetCameraWs() const;

		float GetFovRad() const;

		Rendering::MeshId LoadMesh(Systems::AssetId id);
		Rendering::MaterialId LoadMaterial(Systems::AssetId id);

		bool AddToSelection(const Os::Guid& nodeGuid);
		bool RemoveFromSelection(const Os::Guid& nodeGuid);
		void ClearSelection();

		Systems::AssetId GetCurrentLoadedLevelAssetId() const;
		std::string GetCurrentLoadedLevelName() const;

		//operation callback
		EVENT_DECL(NewLevel, void())
		EVENT_DECL(LoadLevel, void())
		EVENT_DECL(SaveLevel, void())

		EVENT_DECL(AddEntity, void(const Os::Guid& nodeGuid))
		EVENT_DECL(DeleteEntity, void(const Os::Guid& nodeGuid))
		EVENT_DECL(RenameEntity, void(const Os::Guid& nodeGuid))
		EVENT_DECL(DuplicateEntity, void(const Os::Guid& src, const Os::Guid& copy))

		//temp
		std::map<Systems::AssetId, Rendering::MeshId> m_assetIdToMeshId;
		std::map<Systems::AssetId, Rendering::MaterialId> m_assetIdToMaterialId;

	private:
		LevelMgr* m_pLevelMgr;
		SelectionMgr* m_pSelectionMgr;

		Systems::AssetId m_loadedLevelAssetId;

		//camera state
		Core::Mat44f m_cameraWs;
		float m_fovRad;
	};
}
