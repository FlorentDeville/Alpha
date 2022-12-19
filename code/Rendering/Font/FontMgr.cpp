/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Font/FontMgr.h"

#include "Rendering/Font/Font.h"

//Because of some include dependencies, windgi is included and defines CreateFont.
#undef CreateFont

namespace Rendering
{
	FontMgr::FontMgr()
		: Core::Singleton<FontMgr>()
		, m_fonts()
	{}

	FontMgr::~FontMgr()
	{
		for (const Font* pFont : m_fonts)
			delete pFont;
	}

	bool FontMgr::CreateFont(Font** ppFont, FontId& id)
	{
		id = FontId(m_fonts.size());
		m_fonts.push_back(new Font());
		*ppFont = m_fonts.back();

		return true;
	}

	const Font* FontMgr::GetFont(FontId id) const
	{
		assert(id.m_id < m_fonts.size());
		return m_fonts[id.m_id];
	}
}
