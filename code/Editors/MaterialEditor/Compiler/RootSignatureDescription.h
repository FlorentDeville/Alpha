/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"

#include <string>

namespace Editors
{
	struct RootSigParameterIndex
	{
		std::string m_cbufferName;
		int m_rootSigParamIndex;
	};

	struct RootSigTexture
	{
		std::string m_name;
		uint32_t m_rootSigIndex;
	};

	class RootSignatureDescription
	{
	public:
		Core::Array<char>* m_pRootSignatureBlob;

		//List of cbuffer name and its index in the root signature.
		Core::Array<RootSigParameterIndex> m_parameters;
		Core::Array<RootSigTexture> m_textures;
	};
}
