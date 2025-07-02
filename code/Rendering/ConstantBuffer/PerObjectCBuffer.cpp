/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/ConstantBuffer/PerObjectCBuffer.h"

namespace Rendering
{
	PerObjectCBuffer::PerObjectCBuffer(const DirectX::XMMATRIX& worldViewProj)
		: m_worldViewProj(worldViewProj)
	{ }
}
