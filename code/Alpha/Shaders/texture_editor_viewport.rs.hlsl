/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#ifndef TEXTURE_EDITOR_VIEWPORT_RS_HLSL
#define TEXTURE_EDITOR_VIEWPORT_RS_HLSL

#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |" \
					"DENY_HULL_SHADER_ROOT_ACCESS |" \
					"DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
					"DENY_GEOMETRY_SHADER_ROOT_ACCESS)," \
			"RootConstants(num32BitConstants=16, b0, space=0, visibility=SHADER_VISIBILITY_VERTEX)," \
			"RootConstants(num32BitConstants=1, b0, space=0, visibility=SHADER_VISIBILITY_PIXEL)," \
			"DescriptorTable(SRV(t0, space=0))," \
			"DescriptorTable(SRV(t1, space=0))," \
			"StaticSampler(s0, visibility=SHADER_VISIBILITY_PIXEL)" 
			

#endif // #ifndef TEXTURE_EDITOR_VIEWPORT_RS_HLSL
