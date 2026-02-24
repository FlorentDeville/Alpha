/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"
#include "Core/Singleton.h"

#include "Systems/Assets/NewAssetId.h"

namespace Systems
{
	class AssetMetadata;
}

namespace Editors
{
	class MeshEditorModule : public Core::Singleton<MeshEditorModule>
	{
	public:
		MeshEditorModule();
		~MeshEditorModule();

		bool SaveMesh(const Systems::NewAssetId id);
		bool DeleteMesh(const Systems::NewAssetId id);
		bool ImportMesh(const std::string& filename);
		bool RenameMesh(const Systems::NewAssetId id, const std::string& newName);

		EVENT_DECL(MeshCreated, void(const Systems::AssetMetadata& pMetadata));
		EVENT_DECL(BeforeMeshDeleted, void(const Systems::AssetMetadata& pMetadata));
		EVENT_DECL(AfterMeshDeleted, void(const Systems::AssetMetadata& pMetadata));
		EVENT_DECL(MeshRenamed, void(const Systems::AssetMetadata& pMetadata));
	};
}
