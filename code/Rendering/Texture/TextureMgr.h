/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>
#include <vector>

#include "Core/Singleton.h"
#include "Rendering/Texture/TextureId.h"

namespace Rendering
{
	class Texture;

	class TextureMgr : public Core::Singleton<TextureMgr>
	{
	public:
		TextureMgr();
		~TextureMgr();

		bool CreateTexture(Texture** ppTexture, TextureId& id);
		void DeleteTexture(TextureId id);

		const Texture* GetTexture(TextureId id) const;
		Texture* GetTexture(TextureId id);

	private:
		std::vector<Texture*> m_textures;
	};
}
