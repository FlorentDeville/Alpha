/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <string>

#include "Systems/Assets/AssetObjects/Cubemap/Cubemap.h"
#include "Systems/Assets/AssetObjects/Texture/TextureAsset.h"

namespace Importer
{
	class TextureImporter
	{
	public:
		struct Result
		{
			enum Code
			{
				Ok = 0,
				ComError,
				LoadingFailed,
				MipMapFailed,
				CompressionFailed,
				SaveToBlobFailed,
				CubemapCreationFailed,
				WriteDDSFailed,
				BadSize,
				InitFailed,
				SaveToFileFailed
			};

			Code m_code;
			std::string m_msg;

			Result(Code code);
			Result(Code code, const char* pMsg, ...);

			bool IsSuccess() const;
			bool IsFailure() const;
		};

		bool Import(const std::string& sourceFilename, Systems::TextureAsset* pTexture);

		//The source filename must be in the following order : +x -x +y -y +z -z
		TextureImporter::Result ImportCubemap(const std::string sourceFilename[6], Systems::CubemapAsset* pCubemap);

		TextureImporter::Result Export(const std::string& outputFilename, const Systems::TextureAsset* pTexture);
		TextureImporter::Result Export(const std::string& outputFilename, const Systems::CubemapAsset* pCubemap);
	};
}
