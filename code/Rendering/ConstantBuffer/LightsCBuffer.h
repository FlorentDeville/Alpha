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

	// This class needs to be aligned to 16 bytes to match the shader structure.
	// Could be optimized using the paddings to store floats.
	class Light
	{
	public:
		Light();

		void MakeDirectionalLight(const Core::Float3& direction, const Core::Float3& ambient, const Core::Float3& diffuse, const Core::Float3& specular);
		void MakePointLight(const Core::Float3& position, const Core::Float3& ambient, const Core::Float3& diffuse, const Core::Float3& specular,
			float constant, float linear, float quadratic);
		void MakeSpotLight(const Core::Float3& position, const Core::Float3& direction, 
			const Core::Float3& ambient, const Core::Float3& diffuse, const Core::Float3& specular,
			float constant, float linear, float quadratic, float cutOff, float outerCutOff);

	private:
		Core::Float3 m_position;
		float m_outerCutOff;

		Core::Float3 m_direction;
		char m_padding1[4];
		
		Core::Float3 m_ambient;
		char m_padding2[4];
		
		Core::Float3 m_diffuse;
		char m_padding3[4];
		
		Core::Float3 m_specular;
		float m_constant;

		float m_linear;
		float m_quadratic;
		float m_cutOff;
		LightType m_type;
	};

	// This is the structure representing the lights const buffer. It needs to match the structure passed to shaders
	// and defined in includes\CBufferLights.hlsl.
	class LightsCBuffer
	{
	public:
		LightsCBuffer();
		
		Light* AddLight();

		static const int MAX_LIGHT_COUNT = 8;

	private:
		int m_numLight;
		char m_padding[12];
		Light m_lightArray[MAX_LIGHT_COUNT];
	};
}
