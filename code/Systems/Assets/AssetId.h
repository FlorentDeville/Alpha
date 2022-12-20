/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Systems
{
	class AssetId
	{
		friend class AssetMgr;

	public:
		static const AssetId INVALID;

		AssetId();
		explicit AssetId(size_t id);

		bool operator==(const AssetId id) const;
		bool operator!=(const AssetId id) const;
		bool operator<(const AssetId id) const;

	private:
		size_t m_id;
	};
}
