/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Rendering
{
	class MeshId
	{
		friend class MeshMgr;

	public:
		static const MeshId INVALID;

		MeshId();
		explicit MeshId(size_t id);

		bool operator==(const MeshId id) const;
		bool operator!=(const MeshId id) const;

	private:
		size_t m_id;
	};
}
