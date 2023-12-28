/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Core/Callbacks/CallbackList.h"
#include "Core/Math/Mat44f.h"

#include "Editors/LevelEditor/Level.h"

#include <map>

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
	class SelectionMgr;

	class LevelEditorModule : public Core::Singleton<LevelEditorModule>
	{
	public:
		LevelEditorModule();
		~LevelEditorModule();

		void CreateEditor(Widgets::Widget* pParent);

		const Level& GetConstLevel() const;
		Level& GetLevel();

		const SelectionMgr* GetConstSelectionMgr() const;
		SelectionMgr* GetSelectionMgr();

		void AddNewEntity(const std::string& name);
		void DeleteEntity(const Os::Guid& nodeGuid);
		void RenameEntity(const Os::Guid& nodeGuid, const std::string& name);

		void SetCameraWs(const Core::Mat44f& ws);
		const Core::Mat44f& GetCameraWs() const;

		float GetFovRad() const;

		Rendering::MeshId LoadMesh(Systems::AssetId id);

		bool AddToSelection(const Os::Guid& nodeGuid);
		bool RemoveFromSelection(const Os::Guid& nodeGuid);
		void ClearSelection();

		//operation callback
		using OnDeleteEntityEvent = Core::CallbackList<void(const Os::Guid& nodeGuid)>;
		Core::CallbackId OnDeleteEntity(const OnDeleteEntityEvent::Callback& callback);
		void RemoveOnDeleteEntity(Core::CallbackId id);

		using OnRenameEntityEvent = Core::CallbackList<void(const Os::Guid& nodeGuid)>;
		Core::CallbackId OnRenameEntity(const OnRenameEntityEvent::Callback& callback);
		void RemoveOnRenameEntity(Core::CallbackId id);

		//temp
		std::map<Systems::AssetId, Rendering::MeshId> m_assetIdToMeshId;
		std::map<Systems::AssetId, Rendering::MaterialId> m_assetIdToMaterialId;

	private:
		Level m_level;
		SelectionMgr* m_pSelectionMgr;

		//camera state
		Core::Mat44f m_cameraWs;
		float m_fovRad;

		//operation callback
		OnDeleteEntityEvent m_onDeleteEntity;
		OnRenameEntityEvent m_onRenameEntity;
	};
}
