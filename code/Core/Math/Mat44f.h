/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Math/Vec4f.h"

namespace Core
{
	class Mat44f
	{
	public:
		Mat44f();
		Mat44f(const Vec4f& x, const Vec4f& y, const Vec4f& z, const Vec4f& pos);

		const Vec4f& GetX() const;
		const Vec4f& GetY() const;
		const Vec4f& GetZ() const;
		const Vec4f& GetT() const;

		float Get(int row, int column) const;

		void Set(int row, int column, float f);
	private:
		Vec4f m_rows[4];
	};
}
