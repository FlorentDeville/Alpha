/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "Rendering/Mesh/MeshId.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/Renderable/RenderableId.h"

#include "DirectXMath.h"

#include <vector>
#include <string>

class Widget;
namespace Rendering
{
	class RenderTarget;
}

namespace Editors
{
	class MeshEntry
	{
	public:
		std::string m_filename;
		MeshId m_meshId;
		RenderableId m_renderableId;

		MeshEntry();
	};

	class MeshEditor : public Core::Singleton<MeshEditor>
	{
	public:
		MeshEditor();
		~MeshEditor();

		void CreateEditor(Widget* pParent);

		void Update();
		void Render();

	private:
		Rendering::RenderTarget* m_pRenderTarget;

		std::vector<MeshEntry> m_allMeshes;

		int m_selectedMesh; //id in m_allMeshes, -1 if nothing selected

		PipelineStateId m_pid; //pipeline state id to render the mesh

		//camera position
		DirectX::XMVECTOR m_cameraTarget;
		DirectX::XMVECTOR m_cameraEuler;
		float m_cameraDistance;

		float m_aspectRatio;

		bool m_enableViewportControl;
		DirectX::XMUINT2 m_mousePreviousPos;
		bool m_firstFrameMouseDown;

		void ShowMesh(int entryIndex);
		void LoadMesh(const std::string& meshFilename, MeshEntry& entry);

		void OnMeshEntryClicked(int entryIndex);
	};
}
