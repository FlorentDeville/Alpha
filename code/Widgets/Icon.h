/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include "Rendering/Texture/TextureId.h"

#include <string>

namespace Rendering
{
	class Texture;
}

namespace Widgets
{
	class Icon : public Widget
	{
	public:
		Icon();
		Icon(Rendering::TextureId textureId);
		Icon(const std::string& path);
		Icon(const DirectX::XMINT2& pos, const DirectX::XMUINT2 size, const std::string& path);
		~Icon();

		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;

		void SetTextureId(Rendering::TextureId tid);
		void SetTexture(Rendering::Texture* pTexture);

		void Hide();
		void Show();

	private:
		Rendering::Texture* m_pTexture;

		bool m_isHidden;
	};
}