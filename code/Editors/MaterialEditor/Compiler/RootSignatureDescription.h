/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"

#include <map>
#include <string>

namespace Editors
{
	class RootSignatureDescription
	{
	public:
		Core::Array<char>* m_pRootSignatureBlob;

		//Map between a cbuffer name and it's index in the root signature.
		std::map<std::string, int> m_cBufferRootSignatureIndex;
	};
}
