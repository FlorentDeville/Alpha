/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Core
{
	enum FieldAttribute
	{
		None = 0x0,
		Hidden = 1 << 0,			// Hide the field complelety in editors
		DoNotSerialize = 1 << 1,
		ReadOnly = 1 << 2,			// Display the field in editors but keep it readonly
		NoClassDialog = 1 << 3		// Do not show the class selection dialog in array of pointers. Instanciate the template type.
	};
}
