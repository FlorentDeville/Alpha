/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Core
{
	// Define the standard types supported in json.
	enum JsonType
	{
		Undefined = 0,
		Number,
		String,
		Boolean,
		Array,
		Object,
		Null,

		Count
	};
}
