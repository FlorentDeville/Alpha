/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Math/Vec4f.h"

namespace Core
{
	Vec4f::Vec4f()
		: m_x(0)
		, m_y(0)
		, m_z(0)
		, m_w(0)
	{}

	Vec4f::Vec4f(float x, float y, float z, float w)
		: m_x(x)
		, m_y(y)
		, m_z(z)
		, m_w(w)
	{}

	float Vec4f::GetX() const { return m_x; }
	float Vec4f::GetY() const { return m_y; }
	float Vec4f::GetZ() const { return m_z; }
	float Vec4f::GetW() const { return m_w; }
}
