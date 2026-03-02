/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <string>

namespace Widgets
{
	class Widget;
}

namespace Editors
{
	class EditorParameter
	{
	public:
		Widgets::Widget* m_pParent;
		std::string m_shaderPath;
	};
}
