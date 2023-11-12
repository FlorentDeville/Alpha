/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "OsWin/Guid.h"

#include <objbase.h>

namespace Os
{
	Guid::Guid()
		: m_data1(0)
		, m_data2(0)
		, m_data3(0)
		, m_data4(0)
	{}

	Guid::~Guid()
	{}

	bool Guid::operator==(const Guid& other) const
	{
		if (m_data1 != other.m_data1 || m_data2 != other.m_data2 || m_data3 != other.m_data3 || m_data4 != other.m_data4)
			return false;

		return true;
	}

	bool Guid::operator!=(const Guid& other) const
	{
		return !(*this == other);
	}

	Guid Guid::GenerateNewGuid()
	{
		Guid guid;

		GUID winGuid;
		HRESULT res = CoCreateGuid(&winGuid);
		if (res != S_OK)
			return guid;

		guid.m_data1 = winGuid.Data1;
		guid.m_data2 = winGuid.Data2;
		guid.m_data3 = winGuid.Data3;
		memcpy(&guid.m_data4, winGuid.Data4, sizeof(guid.m_data4));

		return guid;
	}
}
