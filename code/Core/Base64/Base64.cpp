/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Core/Base64/Base64.h"

#include <cassert>
#include <cstdlib>
#include <string>

namespace Core
{
	static const int BLOCK_SIZE = 3;

	static std::string ENCODE_LOOKUP_TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	static uint8_t DECODE_LOOKUP_TABLE[] = {
	80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, /* 0 - 15 */
	80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, /* 16 - 31 */
	80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 62, 80, 80, 80, 63, /* [43] = +, [47] = / */
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 80, 80, 80, 64, 80, 80, /* 0 - 9 */
	80,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, /* 64 - 79 */
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 80, 80, 80, 80, 80, /* 80 - 96 */
	80, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* 87 - 111 */
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 80, 80, 80, 80, 80 /* 112 - 127 */
	};

	uint32_t Base64::EncodedSize(uint32_t decodedDataSize)
	{
		std::div_t result = std::div(decodedDataSize, BLOCK_SIZE);
		
		uint32_t blockCount = result.quot;
		if (result.rem != 0)
			++blockCount;

		return blockCount * 4;
	}

	uint32_t Base64::DecodedSize(const char* pData, uint32_t encodedDataSize)
	{
		uint32_t blockCount =  encodedDataSize / 4;
		uint32_t byteSize = blockCount * BLOCK_SIZE;

		if (pData[encodedDataSize - 2] == '=')
			return byteSize - 2;
		else if (pData[encodedDataSize - 1] == '=')
			return byteSize - 1;

		return byteSize;
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
			uint8_t part4 = pBlockStart[2] & 0x3F;

			pBuffer[outputIndex++] = ENCODE_LOOKUP_TABLE[part1];
			pBuffer[outputIndex++] = ENCODE_LOOKUP_TABLE[part2];
			pBuffer[outputIndex++] = ENCODE_LOOKUP_TABLE[part3];
			pBuffer[outputIndex++] = ENCODE_LOOKUP_TABLE[part4];
		}

		if (result.rem == 1)
		{
			//I have 2 bytes left and 1 missing bytes
			int byteStart = fullBlockCount * BLOCK_SIZE;
			const uint8_t* pBlockStart = pData + byteStart;

			uint8_t part1 = pBlockStart[0] >> 2;
			uint8_t part2 = ((pBlockStart[0] & 0x03) << 4) | ((pBlockStart[1] & 0xF0) >> 4);
			uint8_t part3 = ((pBlockStart[1] & 0x0F) << 2);
			
			pBuffer[outputIndex++] = ENCODE_LOOKUP_TABLE[part1];
			pBuffer[outputIndex++] = ENCODE_LOOKUP_TABLE[part2];
			pBuffer[outputIndex++] = ENCODE_LOOKUP_TABLE[part3];
			pBuffer[outputIndex++] = '=';
		}
		else if (result.rem == 2)
		{
			//I have 1 byte left and 2 missing bytes
			int byteStart = fullBlockCount * BLOCK_SIZE;
			const uint8_t* pBlockStart = pData + byteStart;

			uint8_t part1 = pBlockStart[0] >> 2;
			uint8_t part2 = ((pBlockStart[0] & 0x03) << 4);

			pBuffer[outputIndex++] = ENCODE_LOOKUP_TABLE[part1];
			pBuffer[outputIndex++] = ENCODE_LOOKUP_TABLE[part2];
			pBuffer[outputIndex++] = '=';
			pBuffer[outputIndex++] = '=';
		}
	}

	void Base64::Decode(const char* pData, uint32_t dataSize, uint8_t* pBuffer, uint32_t bufferSize)
	{
		const int CHAR_PER_BLOCK = 4;
		const uint32_t fullBlockCount = dataSize / CHAR_PER_BLOCK;

		uint32_t outputIndex = 0;

		for (uint32_t blockIndex = 0; blockIndex < fullBlockCount - 1; ++blockIndex)
		{
			int byteIndexStart = blockIndex * CHAR_PER_BLOCK;
			const char* pBlockStart = pData + byteIndexStart;

			uint8_t b1 = DECODE_LOOKUP_TABLE[pBlockStart[0]];
			uint8_t b2 = DECODE_LOOKUP_TABLE[pBlockStart[1]];
			uint8_t b3 = DECODE_LOOKUP_TABLE[pBlockStart[2]];
			uint8_t b4 = DECODE_LOOKUP_TABLE[pBlockStart[3]];

			pBuffer[outputIndex++] = (b1 << 2 | ((b2 & 0xF0) >> 4));
			pBuffer[outputIndex++] = (((b2 & 0x0F) << 4) | ((b3 & 0x3C) >> 2));
			pBuffer[outputIndex++] = (((b3 & 0x03) << 6) | (b4 & 0x3F));
		}

		if (pData[dataSize - 2] == '=')
		{
			//I have 1 byte left and 2 missing bytes
			int byteStart = (fullBlockCount - 1) * CHAR_PER_BLOCK;
			const char* pBlockStart = pData + byteStart;

			uint8_t b1 = DECODE_LOOKUP_TABLE[pBlockStart[0]];
			uint8_t b2 = DECODE_LOOKUP_TABLE[pBlockStart[1]];
			pBuffer[outputIndex++] = (b1 << 2 | ((b2 & 0xF0) >> 4));
		}
		else if (pData[dataSize - 1] == '=')
		{
			//I have 2 byte lefts and 1 missing bytes
			int byteStart = (fullBlockCount - 1) * CHAR_PER_BLOCK;
			const char* pBlockStart = pData + byteStart;

			uint8_t b1 = DECODE_LOOKUP_TABLE[pBlockStart[0]];
			uint8_t b2 = DECODE_LOOKUP_TABLE[pBlockStart[1]];
			uint8_t b3 = DECODE_LOOKUP_TABLE[pBlockStart[2]];

			pBuffer[outputIndex++] = (b1 << 2 | ((b2 & 0xF0) >> 4));
			pBuffer[outputIndex++] = (((b2 & 0x0F) << 4) | ((b3 & 0x3C) >> 2));
		}

		assert(outputIndex <= bufferSize);
	}
}
