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
		std::string m_strType;
		uint32_t m_size;
		uint32_t m_offset;
	};

	class MaterialParameters
	{
	public:
		MaterialParameters();

		std::vector<MaterialParameter> m_perMaterialParameters;
	};
}
