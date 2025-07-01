/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Sid/Sid.h"

#include <string>
#include <vector>

namespace Editors
{
	class MaterialParameter
	{
	public:
		std::string m_name;
		Core::Sid m_type;
		uint32_t m_size;
		uint32_t m_offset;
	};

	class MaterialParameters
	{
	public:
		MaterialParameters();

		bool m_hasPerObjectParameters;
		bool m_hasPerFrameParameters;
		bool m_hasPerMaterialParameters;

		std::vector<MaterialParameter> m_perMaterialParameters;
	};
}
