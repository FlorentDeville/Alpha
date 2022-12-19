/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Rendering
{
	class FontId
	{
		friend class FontMgr;

	public:
		static const FontId INVALID;

		FontId();
		explicit FontId(size_t id);

		bool operator==(const FontId id) const;
		bool operator<(const FontId id) const;

	private:
		size_t m_id;
	};
}
