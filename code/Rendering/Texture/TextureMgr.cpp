/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Texture/TextureMgr.h"

#include "Rendering/Texture/Texture.h"

#include <assert.h>

//#pragma optimize("", off)

namespace Rendering
{
	TextureMgr::TextureMgr()
		: Core::Singleton<TextureMgr>()
		, m_textures()
	{}

	TextureMgr::~TextureMgr()
	{
		for (const Texture* pTexture : m_textures)
			delete pTexture;
	}

	bool TextureMgr::CreateTexture(Texture** ppTexture, TextureId& id)
	{
		id = TextureId(m_textures.size());
		m_textures.push_back(new Texture());
		*ppTexture = m_textures.back();

		return true;
	}

	void TextureMgr::DeleteTexture(TextureId id)
	{
		assert(id.m_id < m_textures.size());
		Texture* pTexture = m_textures[id.m_id];
		delete pTexture;
		m_textures[id.m_id] = nullptr;
	}

	const Texture* TextureMgr::GetTexture(TextureId id) const
	{
		assert(id.m_id < m_textures.size());
		return m_textures[id.m_id];
	}

	Texture* TextureMgr::GetTexture(TextureId id)
	{
		assert(id.m_id < m_textures.size());
		return m_textures[id.m_id];
	}
}
