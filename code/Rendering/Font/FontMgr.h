/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <vector>

#include "Core/Singleton.h"
#include "Rendering/Font/FontId.h"

#undef CreateFont

namespace Rendering
{
	class Font;

	class FontMgr : public Core::Singleton<FontMgr>
	{
	public:
		FontMgr();
		~FontMgr();

		bool CreateFont(Font** ppFont, FontId& id);

		const Font* GetFont(FontId id) const;

	private:
		std::vector<Font*> m_fonts;
	};
}