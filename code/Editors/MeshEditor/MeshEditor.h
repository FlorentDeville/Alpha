/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "Rendering/Renderable/RenderableId.h"

#include <vector>
#include <string>

class Widget;
namespace Rendering
{
	class RenderTarget;
}

namespace Editors
{
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

		std::vector<std::string> m_meshesFilenameList;

		RenderableId m_meshToRender;

		void LoadMesh(const std::string& meshFilename);
	};
}
