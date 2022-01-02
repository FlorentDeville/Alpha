/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#ifndef TEXTURE_RS_HLSL
#define TEXTURE_RS_HLSL

#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |" \
					"DENY_HULL_SHADER_ROOT_ACCESS |" \
					"DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
					"DENY_GEOMETRY_SHADER_ROOT_ACCESS)," \
			"RootConstants(num32BitConstants=16, b0, space=0, visibility=SHADER_VISIBILITY_VERTEX)," \
			"RootConstants(num32BitConstants=2, b1, space=0, visibility=SHADER_VISIBILITY_VERTEX)," \
			"RootConstants(num32BitConstants=2, b2, space=0, visibility=SHADER_VISIBILITY_VERTEX)," \
			"StaticSampler(s0, visibility=SHADER_VISIBILITY_PIXEL), " \
			"DescriptorTable(SRV(t0, space=0)),"


#endif
