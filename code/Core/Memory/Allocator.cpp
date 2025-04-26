/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Memory/Allocator.h"

namespace Core
{
	Allocator::Allocator()
		: m_pStart(nullptr)
		, m_size()
	{ }

	Allocator::Allocator(void* pStart, size_t size)
		: m_pStart(reinterpret_cast<char*>(pStart))
		, m_size(size)
	{ }

	Allocator::~Allocator()
	{ }

	void Allocator::Init(void* pStart, size_t size)
	{
		m_pStart = reinterpret_cast<char*>(pStart);
		m_size = size;
	}

	uint16_t Allocator::ComputeAlignmentOffset(const void* pPtr, uint16_t alignment) const
	{
		// Alignment must be a power of 2 so only 1 bit is set to 1, all others are 0. So alignment - 1
		// flips the 1 into a 0 and flips the folling 0 into a 1.
		// For example 4 is 0b0000 0100 and 4 - 1 is 0b0000 0011.
		// Masking off the bits set to 1 in pPtr will create an aligned address. So I need to flip the bits again
		// before masking.
		if (alignment == 0)
			return 0;

		uint64_t alignMinusOne = alignment - 1;
		uint64_t flipAlignMinusOne = ~alignMinusOne;

		uint64_t maskedAddress = reinterpret_cast<const uint64_t>(pPtr) & flipAlignMinusOne;

		if (maskedAddress == reinterpret_cast<const uint64_t>(pPtr))
			return 0;

		uint64_t offset = alignment - (reinterpret_cast<const uint64_t>(pPtr) - maskedAddress);
		//uint8_t offset = static_cast<uint8_t>(maskedAddress - reinterpret_cast<const uint64_t>(pPtr));
		return static_cast<uint16_t>(offset);
	}
}
