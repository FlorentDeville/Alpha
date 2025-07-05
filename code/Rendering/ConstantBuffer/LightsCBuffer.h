/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <DirectXMath.h>

namespace Rendering
{
	enum LightType : int
	{
		Directional = 0,
		Point,
		Spot,

		Count
	};

	class Light
	{
	public:
		Light();

		static Light MakeDirectionalLight(const DirectX::XMFLOAT3& direction);

	private:
		DirectX::XMFLOAT3 m_position;
		char m_padding0[4];
		DirectX::XMFLOAT3 m_direction;
		float m_coneAngle;
		LightType m_type;
		char m_padding1[12];
	};

	// This is the structure representing the lights const buffer. It needs to match the structure passed to shaders
	// and defined in includes\CBufferLights.hlsl.
	class LightsCBuffer
	{
	public:
		LightsCBuffer(const Light& light);

	private:
		int m_numLight;
		char m_padding[12];
		Light m_lightArray[8];
	};
}
