/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Vec4f.h"

class PlayerStateContext
{
public:

	//Relative position of the dash target with respect to the player position.
	//This is set PREPARE_DASH and used in DASH.
	Core::Vec4f m_dashTargetRelativePosition;

};
