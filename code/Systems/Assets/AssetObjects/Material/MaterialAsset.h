/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Systems/Objects/AssetObject.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, MaterialAsset_v1)
	class MaterialAsset_v1 : public AssetObject
	{
	public:
		MaterialAsset_v1();

	private:
		std::string m_sourceFilePS;
		std::string m_sourceFileVS;
		std::string m_sourceFileRS;

		Core::Array<char> m_psBlob;
		Core::Array<char> m_vsBlob;
		Core::Array<char> m_rsBlob;

		START_REFLECTION(Systems::MaterialAsset_v1)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_sourceFilePS)
			ADD_FIELD(m_sourceFileVS)
			ADD_FIELD(m_sourceFileRS)
			ADD_FIELD_ATTR(m_psBlob, DoNotEdit)
			ADD_FIELD_ATTR(m_vsBlob, DoNotEdit)
			ADD_FIELD_ATTR(m_rsBlob, DoNotEdit)
		END_REFLECTION()
	};

	using MaterialAsset = MaterialAsset_v1;
}
