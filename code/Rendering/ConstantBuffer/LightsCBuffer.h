/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Math/Vectors.h"

namespace Rendering
{
	enum LightType : int
	{
		Unknown = -1,
		Directional = 0,
		Point,
		Spot,

		Count
	};

	class Light
	{
	public:
		Light();

		static Light MakeDirectionalLight(const Core::Float3& direction);

	private:
		Core::Float3 m_position;
		char m_padding0[4];
		Core::Float3 m_direction;
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
