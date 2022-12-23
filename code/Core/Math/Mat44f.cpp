/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Math/Mat44f.h"

namespace Core
{
	Mat44f::Mat44f()
	{
		m_rows[0] = Vec4f();
		m_rows[1] = Vec4f();
		m_rows[2] = Vec4f();
		m_rows[3] = Vec4f();
	}

	Mat44f::Mat44f(const Vec4f& x, const Vec4f& y, const Vec4f& z, const Vec4f& pos)
	{
		m_rows[0] = x;
		m_rows[1] = y;
		m_rows[2] = z;
		m_rows[3] = pos;
	}

	const Vec4f& Mat44f::GetX() const { return m_rows[0]; }
	const Vec4f& Mat44f::GetY() const { return m_rows[1]; }
	const Vec4f& Mat44f::GetZ() const { return m_rows[2]; }
	const Vec4f& Mat44f::GetT() const { return m_rows[3]; }
}
