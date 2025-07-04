/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <DirectXMath.h>

namespace Rendering
{
	class PerFrameCBuffer
	{
	public:
		PerFrameCBuffer(const DirectX::XMMATRIX& view, DirectX::XMMATRIX& proj);

	private:
		DirectX::XMMATRIX m_viewMatrix;
		DirectX::XMMATRIX m_projMatrix;
	};
}
