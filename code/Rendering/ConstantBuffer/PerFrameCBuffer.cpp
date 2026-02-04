/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/ConstantBuffer/PerFrameCBuffer.h"

namespace Rendering
{
	PerFrameCBuffer::PerFrameCBuffer(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, const DirectX::XMFLOAT3& cameraPos)
		: m_viewMatrix(view)
		, m_projMatrix(proj)
		, m_cameraPosition(cameraPos)
	{ }
}
