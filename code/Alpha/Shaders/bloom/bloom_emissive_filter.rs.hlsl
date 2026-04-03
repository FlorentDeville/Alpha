/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#ifndef BLOOM_EMISSIVE_RS_HLSL
#define BLOOM_EMISSIVE_RS_HLSL

#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |" \
					"DENY_HULL_SHADER_ROOT_ACCESS |" \
					"DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
					"DENY_GEOMETRY_SHADER_ROOT_ACCESS)," \
					"StaticSampler(s0, visibility=SHADER_VISIBILITY_PIXEL), " \
					"DescriptorTable(SRV(t0, space=0))"

#endif
