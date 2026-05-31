/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#ifndef UI_RS_HLSL
#define UI_RS_HLSL

#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
            "RootConstants(num32BitConstants=32, b0, visibility=SHADER_VISIBILITY_VERTEX)," \
            "RootConstants(num32BitConstants=4, b1, visibility=SHADER_VISIBILITY_VERTEX)," \
            "DescriptorTable(SRV(t0))," \
            "StaticSampler(s0, visibility=SHADER_VISIBILITY_PIXEL, addressU=TEXTURE_ADDRESS_CLAMP, addressV=TEXTURE_ADDRESS_CLAMP, addressW=TEXTURE_ADDRESS_CLAMP)"

#endif // #ifndef UI_RS_HLSL