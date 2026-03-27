/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#ifndef COPY_RENDER_TARGET_RS_HLSL
#define COPY_RENDER_TARGET_RS_HLSL

#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
			"StaticSampler(s0, visibility=SHADER_VISIBILITY_PIXEL), " \
			"DescriptorTable(SRV(t0, space=0)),"

#endif // #ifndef COPY_RENDER_TARGET_RS_HLSL
