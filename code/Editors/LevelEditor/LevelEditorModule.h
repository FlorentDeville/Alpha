/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Core/Callbacks/CallbackList.h"
#include "Core/Math/Mat44f.h"

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

		void CreateEditor(Widgets::Widget* pParent);

		const LevelMgr* GetConstLevelMgr() const;
		LevelMgr* GetLevelMgr();

		const SelectionMgr* GetConstSelectionMgr() const;
		SelectionMgr* GetSelectionMgr();

		void AddNewEntity(Os::Guid& nodeGuid);
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
		using OnAddEntityEvent = Core::CallbackList<void(const Os::Guid& nodeGuid)>;
		Core::CallbackId OnAddEntity(const OnAddEntityEvent::Callback& callback);
		void RemoveOnAddEntity(Core::CallbackId id);

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
		LevelMgr* m_pLevelMgr;
		SelectionMgr* m_pSelectionMgr;

		//camera state
		Core::Mat44f m_cameraWs;
		float m_fovRad;

		//operation callback
		OnAddEntityEvent	m_onAddEntity;
		OnDeleteEntityEvent m_onDeleteEntity;
		OnRenameEntityEvent m_onRenameEntity;
	};
}
