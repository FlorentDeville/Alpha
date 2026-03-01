/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Objects/AssetObject.h"

#include "Systems/Reflection/ReflectionMacro.h"

namespace Rendering
{
	class Texture;
}

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, CubemapAsset_v1)
	class CubemapAsset_v1 : public AssetObject
	{
	public:
		CubemapAsset_v1();
		~CubemapAsset_v1();

		bool Init(const std::string sourceFilename[6], const uint8_t* pBlob, uint32_t blobSize);

		void PostLoad() override;

		static const std::string& GetAssetTypeName();
		static Core::Sid GetAssetTypeNameSid();

		const std::string& GetLeftSourceFilename() const;
		const std::string& GetRightSourceFilename() const;
		const std::string& GetTopSourceFilename() const;
		const std::string& GetBottomSourceFilename() const;
		const std::string& GetFrontSourceFilename() const;
		const std::string& GetBackSourceFilename() const;

		Rendering::Texture* GetTexture();

		const Core::Array<uint8_t>& GetBlob() const;

	private:
		std::string m_leftSourceFilename; //-x
		std::string m_rightSourceFilename; //+x
		std::string m_topSourceFilename; //+y
		std::string m_bottomSourceFilename; //-y
		std::string m_frontSourceFilename; //+z
		std::string m_backSourceFilename; //-z
		
		Core::Array<uint8_t> m_blob;

		START_REFLECTION(Systems::CubemapAsset_v1)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_leftSourceFilename)
			ADD_FIELD(m_rightSourceFilename)
			ADD_FIELD(m_topSourceFilename)
			ADD_FIELD(m_bottomSourceFilename)
			ADD_FIELD(m_frontSourceFilename)
			ADD_FIELD(m_backSourceFilename)
			ADD_FIELD_ATTR(m_blob, Hidden)
		END_REFLECTION()

		Rendering::Texture* m_pTexture;
	};
}
