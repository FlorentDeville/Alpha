/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

#include "Systems/Assets/AssetId.h"
#include "Systems/Assets/AssetType.h"

namespace Systems
{
	class Asset
	{
	public:
		Asset(AssetId id, const std::string& virtualName, const std::string& path, AssetType type);
		~Asset();

		AssetId GetId() const;
		const std::string& GetVirtualName() const;
		const std::string& GetPath() const;
		AssetType GetType() const;

	private:
		std::string m_virtualName;
		std::string m_path;
		AssetId m_id;
		AssetType m_type;
	};
}
