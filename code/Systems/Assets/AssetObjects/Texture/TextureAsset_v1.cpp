/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetObjects/Texture/TextureAsset_v1.h"

namespace Systems
{
	TextureAsset_v1::TextureAsset_v1()
		: AssetObject()
		, m_sourceFile()
		, m_blob()
	{ }

	TextureAsset_v1::~TextureAsset_v1()
	{ }

	void TextureAsset_v1::PostLoad()
	{ }

	const std::string& TextureAsset_v1::GetAssetTypeName()
	{
		static std::string name = "Texture";
		return name;
	}

	Core::Sid TextureAsset_v1::GetAssetTypeNameSid()
	{
		static Core::Sid sid = SID(GetAssetTypeName());
		return sid;
	}
}
