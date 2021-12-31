/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#ifndef TEXT_RS_HLSL
#define TEXT_RS_HLSL

#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |" \
					"DENY_HULL_SHADER_ROOT_ACCESS |" \
					"DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
					"DENY_GEOMETRY_SHADER_ROOT_ACCESS)," \
					"StaticSampler(s0, filter=FILTER_MIN_MAG_MIP_POINT, visibility=SHADER_VISIBILITY_PIXEL), " \
					"DescriptorTable(SRV(t0, space=0)),"


#endif
