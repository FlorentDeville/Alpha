/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <string>

#include "Systems/Assets/AssetObjects/Cubemap/CubemapAsset.h"
//#include "Systems/Assets/AssetObjects/Texture/Texture2DAsset.h"

namespace Systems
{
	class Texture2DAsset;
}

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
				WicFactoryError,
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

		bool Import(const std::string& sourceFilename, Systems::Texture2DAsset* pTexture);

		//The source filename must be in the following order : +x -x +y -y +z -z
		TextureImporter::Result ImportCubemap(const std::string sourceFilename[6], Systems::CubemapAsset* pCubemap);

		TextureImporter::Result Export(const std::string& outputFilename, const Systems::Texture2DAsset* pTexture);
		TextureImporter::Result Export(const std::string& outputFilename, const Systems::CubemapAsset* pCubemap);
	};
}
