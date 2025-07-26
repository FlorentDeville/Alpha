/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Systems
{
	enum FieldAttribute
	{
		None = 0x0,
		Hidden = 0x1,			// Hide the field complelety in editors
		DoNotSerialize = 0x2,
		ReadOnly = 0x04			// Display the field in editors but keep it readonly
	};
}
