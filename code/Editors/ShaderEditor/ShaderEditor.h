/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include <string>
#include <vector>

class Widget;
namespace Widgets
{
	class Layout;
}

namespace Editors
{
	class ShaderEntry
	{
	public:
		std::string m_filename;

		ShaderEntry();
	};

	class ShaderEditor : public Core::Singleton<ShaderEditor>
	{
	public:
		ShaderEditor();
		~ShaderEditor();

		void CreateEditor(Widget* pParent);

		void Update();
		void Render();

	private:

		std::vector<ShaderEntry> m_allShaders;

		Widgets::Layout* m_pShaderListLayout;

		int m_selectedShader;

		bool OnShaderEntryClicked(int index);
		bool OnCompileClicked();
	};
}
