/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Objects/AssetObject.h"

namespace Core
{
	class Blob;
}

namespace Rendering
{
	class Texture;
}

namespace Systems
{
	//Interface for any kind of texture (Texture2D, Cubemap, etc).
	//It's a pure interface and MUST NOT contain any data. If this class contains data and 
	//its data are changed, the serialization will break.
	class ITextureAsset : public AssetObject
	{
	public:
		ITextureAsset() = default;
		~ITextureAsset() = default;

		virtual Rendering::Texture* GetTexture() = 0;
		virtual const Core::Blob& GetBlob() const = 0;
	};
}
