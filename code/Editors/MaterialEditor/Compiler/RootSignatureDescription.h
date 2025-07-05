/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"

#include <map>
#include <string>

namespace Editors
{
	struct RootSigParameterIndex
	{
		std::string m_cbufferName;
		int m_rootSigParamIndex;
	};

	class RootSignatureDescription
	{
	public:
		Core::Array<char>* m_pRootSignatureBlob;

		//List of cbuffer name and its index in the root signature.
		std::vector<RootSigParameterIndex> m_parameters;
	};
}
