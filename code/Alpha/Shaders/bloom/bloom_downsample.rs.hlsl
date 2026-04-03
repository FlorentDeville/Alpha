/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#ifndef BLOOM_DOWNSAMPLE_RS_HLSL
#define BLOOM_DOWNSAMPLE_RS_HLSL

#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |" \
					"DENY_HULL_SHADER_ROOT_ACCESS |" \
					"DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
					"DENY_GEOMETRY_SHADER_ROOT_ACCESS)," \
					"RootConstants(num32BitConstants=2, b0, space=0, visibility=SHADER_VISIBILITY_PIXEL)," \
					"StaticSampler(s0, filter=FILTER_MIN_MAG_MIP_LINEAR, addressU=TEXTURE_ADDRESS_CLAMP, "\
										"addressV=TEXTURE_ADDRESS_CLAMP, "\
										"addressW=TEXTURE_ADDRESS_CLAMP,"\
										"visibility=SHADER_VISIBILITY_PIXEL), " \
					"DescriptorTable(SRV(t0, space=0))"

#endif
