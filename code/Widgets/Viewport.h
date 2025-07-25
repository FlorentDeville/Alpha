/********************************************************************/
/* � 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include "Rendering/Texture/TextureId.h"

namespace Widgets
{
	class Viewport : public Widget
	{
	public:
		Viewport();
		~Viewport();

		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;

		void OnGetRenderTargetTexture(const std::function<Rendering::TextureId ()>& callback);

	private:
		std::function<Rendering::TextureId ()> m_onGetRenderTargetTexture;		//callback to retrieve the texture to show in the viewport
	};
}