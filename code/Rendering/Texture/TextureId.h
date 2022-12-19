/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Rendering
{
	class TextureId
	{
		friend class TextureMgr;

	public:
		static const TextureId INVALID;

		TextureId();
		explicit TextureId(size_t id);

		bool operator==(const TextureId id) const;
		bool operator!=(const TextureId id) const;

	private:
		size_t m_id;
	};
}