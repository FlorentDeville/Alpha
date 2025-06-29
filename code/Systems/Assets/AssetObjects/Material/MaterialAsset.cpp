/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"

namespace Systems
{
	MaterialAsset_v1::MaterialAsset_v1()
		: m_sourceFilePS()
		, m_sourceFileRS()
		, m_sourceFileVS()
	{ }

	const std::string& MaterialAsset_v1::GetSourceFilePs() const
	{
		return m_sourceFilePS;
	}

	Core::Array<char>& MaterialAsset_v1::GetPsBlob()
	{
		return m_psBlob;
	}
}
