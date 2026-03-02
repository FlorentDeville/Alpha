/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"

namespace Core
{
	class Blob
	{
	public:
		Blob();
		~Blob();

		const uint8_t* GetData() const;
		uint8_t* GetData();

		uint32_t GetSize() const;

		void Resize(uint32_t size);

	private:
		Core::Array<uint8_t> m_blob;
	};
}
