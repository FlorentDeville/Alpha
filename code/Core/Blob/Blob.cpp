/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Core/Blob/Blob.h"

namespace Core
{
	Blob::Blob()
		: m_blob()
	{ }

	Blob::~Blob()
	{ }

	const uint8_t* Blob::GetData() const
	{
		return m_blob.GetData();
	}

	uint8_t* Blob::GetData()
	{
		return m_blob.GetData();
	}

	uint32_t Blob::GetSize() const
	{
		return m_blob.GetSize();
	}

	void Blob::Resize(uint32_t size)
	{
		m_blob.Resize(size);
	}
}
