/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

#include "Systems/Assets/AssetId.h"
#include "Systems/Assets/AssetType.h"

namespace Systems
{
	// An asset only contains the metadata for a resource loaded by the game. When you load an asset, you don't load any content usable by the game,
	// you only load information about the asset.
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
