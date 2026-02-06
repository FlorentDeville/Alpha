/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#ifndef BASE_RS_HLSL
#define BASE_RS_HLSL

#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |" \
					"DENY_HULL_SHADER_ROOT_ACCESS |" \
					"DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
					"DENY_GEOMETRY_SHADER_ROOT_ACCESS |" \
					"DENY_PIXEL_SHADER_ROOT_ACCESS)," \
			"RootConstants(num32BitConstants=20, b0, space=0, visibility=SHADER_VISIBILITY_VERTEX)," \
			"RootConstants(num32BitConstants=32, b1, space=0, visibility=SHADER_VISIBILITY_VERTEX)"

#endif
