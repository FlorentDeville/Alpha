/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <DirectXMath.h>

namespace Rendering
{
	// This is the structure repsrenting the per object const buffer. It needs to match the structure passed to shaders
	// and defined in includes\CBufferPerObject.hlsl.
	class PerObjectCBuffer
	{
	public:
		PerObjectCBuffer(const DirectX::XMMATRIX& worldViewProj);

	private:
		DirectX::XMMATRIX m_worldViewProj;
	};
}
