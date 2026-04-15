/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#ifndef PARTICLES_RS_HLSL
#define PARTICLES_RS_HLSL

#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
            "DescriptorTable(SRV(t0)), " \
            "RootConstants(num32BitConstants=32, b0, visibility=SHADER_VISIBILITY_VERTEX)"

#endif // #ifndef PARTICLES_RS_HLSL