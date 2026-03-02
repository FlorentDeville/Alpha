/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Core/Base64/Base64.h"

#include <cstdlib>
#include <string>

namespace Core
{
	const int BLOCK_SIZE = 3;

	static std::string LOOKUP_TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	uint32_t Base64::EncodedSize(uint32_t decodedDataSize)
	{
		std::div_t result = std::div(decodedDataSize, BLOCK_SIZE);
		
		uint32_t blockCount = result.quot;
		if (result.rem != 0)
			++blockCount;

		return blockCount * 4;
	}

	void Base64::Encode(const uint8_t* pData, uint32_t size, char* pBuffer)
	{
		std::div_t result = std::div(size, BLOCK_SIZE);

		const uint32_t fullBlockCount = result.quot;

		uint32_t outputIndex = 0;

		for (uint32_t blockIndex = 0; blockIndex < fullBlockCount; ++blockIndex)
		{
			int byteIndexStart = blockIndex * BLOCK_SIZE;
			const uint8_t* pBlockStart = pData + byteIndexStart;
			
			uint8_t part1 = pBlockStart[0] >> 2;
			uint8_t part2 = ((pBlockStart[0] & 0x03) << 4) | ((pBlockStart[1] & 0xF0) >> 4);
			uint8_t part3 = ((pBlockStart[1] & 0x0F) << 2) | ((pBlockStart[2] & 0xC0) >> 6);
			uint8_t part4 = pBlockStart[3] & 0x3F;

			pBuffer[outputIndex++] = LOOKUP_TABLE[part1];
			pBuffer[outputIndex++] = LOOKUP_TABLE[part2];
			pBuffer[outputIndex++] = LOOKUP_TABLE[part3];
			pBuffer[outputIndex++] = LOOKUP_TABLE[part4];
		}

		if (result.rem == 1)
		{
			//I have 2 bytes left and 1 missing bytes
			int byteStart = fullBlockCount * BLOCK_SIZE;
			const uint8_t* pBlockStart = pData + byteStart;

			uint8_t part1 = pBlockStart[0] >> 2;
			uint8_t part2 = ((pBlockStart[0] & 0x03) << 4) | ((pBlockStart[1] & 0xF0) >> 4);
			uint8_t part3 = ((pBlockStart[1] & 0x0F) << 2);
			
			pBuffer[outputIndex++] = LOOKUP_TABLE[part1];
			pBuffer[outputIndex++] = LOOKUP_TABLE[part2];
			pBuffer[outputIndex++] = LOOKUP_TABLE[part3];
			pBuffer[outputIndex++] = '=';
		}
		else if (result.rem == 2)
		{
			//I have 1 byte left and 2 missing bytes
			int byteStart = fullBlockCount * BLOCK_SIZE;
			const uint8_t* pBlockStart = pData + byteStart;

			uint8_t part1 = pBlockStart[0] >> 2;
			uint8_t part2 = ((pBlockStart[0] & 0x03) << 4);

			pBuffer[outputIndex++] = LOOKUP_TABLE[part1];
			pBuffer[outputIndex++] = LOOKUP_TABLE[part2];
			pBuffer[outputIndex++] = '=';
			pBuffer[outputIndex++] = '=';
		}
	}
}
