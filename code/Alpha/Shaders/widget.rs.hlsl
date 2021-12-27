/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#ifndef WIDGET_RS_HLSL
#define WIDGET_RS_HLSL

#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |" \
					"DENY_HULL_SHADER_ROOT_ACCESS |" \
					"DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
					"DENY_GEOMETRY_SHADER_ROOT_ACCESS)," \
			"RootConstants(num32BitConstants=16, b0, space=0, visibility=SHADER_VISIBILITY_VERTEX)," \
			"RootConstants(num32BitConstants=4, b1, space=0, visibility=SHADER_VISIBILITY_PIXEL)," \
			"RootConstants(num32BitConstants=1, b2, space=0, visibility=SHADER_VISIBILITY_PIXEL)," \
			"RootConstants(num32BitConstants=4, b3, space=0, visibility=SHADER_VISIBILITY_PIXEL)"

#endif
