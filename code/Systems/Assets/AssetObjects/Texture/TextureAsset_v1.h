/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Objects/AssetObject.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, TextureAsset_v1)
	class TextureAsset_v1 : public AssetObject
	{
	public:
		TextureAsset_v1();
		~TextureAsset_v1();

		void PostLoad() override;

		static const std::string& GetAssetTypeName();
		static Core::Sid GetAssetTypeNameSid();

	private:

		std::string m_sourceFile;
		Core::Array<char> m_blob;

		START_REFLECTION(Systems::TextureAsset_v1)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_sourceFile)
			ADD_FIELD(m_blob)
		END_REFLECTION()
	};
}
