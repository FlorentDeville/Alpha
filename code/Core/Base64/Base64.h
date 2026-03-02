/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

namespace Core
{
	class Base64
	{
	public:
		//Compute the size of the encoded string from the size of the decoded data size.
		static uint32_t EncodedSize(uint32_t decodedDataSize);

		static void Encode(const uint8_t* pData, uint32_t size, char* pBuffer);
	};
}
