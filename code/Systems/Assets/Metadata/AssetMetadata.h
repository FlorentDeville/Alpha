/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Sid/Sid.h"

#include "Systems/Assets/NewAssetId.h"

#include <string>

namespace Systems
{
	class AssetMetadata
	{
	public:
		AssetMetadata();
		AssetMetadata(NewAssetId id, const std::string& virtualName, Core::Sid assetType);
		~AssetMetadata();

		NewAssetId GetAssetId() const;
		const std::string GetVirtualName() const;
		Core::Sid GetAssetType() const;

		void SetVirtualName(const std::string& virtualName);

		template<typename T> bool IsA() const;

	private:
		NewAssetId m_id;
		std::string m_virtualName;
		Core::Sid m_assetType;
	};

	template<typename T> bool AssetMetadata::IsA() const
	{
		return m_assetType == T::GetAssetTypeNameSid();
	}
}
