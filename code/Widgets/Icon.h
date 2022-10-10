/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include "Rendering/Texture/Texture.h"

#include <string>

namespace Widgets
{
	class Icon : public Widget
	{
	public:
		Icon(const DirectX::XMINT2& pos, const DirectX::XMUINT2 size, const std::string& path);
		~Icon();

		void Draw(const DirectX::XMFLOAT2& windowSize) override;

	private:
		TextureId m_textureId;
	};
}