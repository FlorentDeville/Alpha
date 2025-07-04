/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MaterialEditor/Compiler/MaterialParameters.h"

namespace Editors
{
	MaterialParameters::MaterialParameters()
		: m_hasPerFrameParameters(false)
		, m_hasPerObjectParameters(false)
		, m_hasPerMaterialParameters(false)
		, m_hasLightsCBufferParameters(false)
		, m_perMaterialParameters()
	{ }
}
