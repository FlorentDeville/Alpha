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
		PerFrameCBuffer(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, const DirectX::XMFLOAT3& cameraPos);

	private:
		DirectX::XMMATRIX m_viewMatrix;
		DirectX::XMMATRIX m_projMatrix;
		DirectX::XMFLOAT3 m_cameraPosition;
	};
}
