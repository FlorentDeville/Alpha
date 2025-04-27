/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Core
{
	class StringView
	{
	public:
		StringView();
		StringView(const char* pPtr, uint32_t size);

		StringView Substr(uint32_t start) const;
		StringView Substr(uint32_t start, uint32_t length) const;

		bool Empty() const;

		char Front() const;
		const char* GetData() const;
		uint32_t GetSize() const;

		char operator[](uint32_t index) const;

		uint32_t FindFirstOf(char c, uint32_t pos = 0) const;

	private:
		const char* m_pPtr;
		uint32_t m_size;
	};
}
