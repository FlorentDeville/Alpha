/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

class Widget;
namespace Rendering
{
	class RenderTarget;
}

namespace Editors
{
	class LevelEditor : public Core::Singleton<LevelEditor>
	{
	public:
		LevelEditor();
		~LevelEditor();

		void CreateEditor(Widget* pParent);

		void Update();
		void Render();

	private:
		Rendering::RenderTarget* m_pRenderTarget;
	};
}