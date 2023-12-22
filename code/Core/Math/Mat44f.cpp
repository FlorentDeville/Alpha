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
	const Vec4f& Mat44f::GetRow(int row) const { return m_rows[row]; }

	float Mat44f::Get(int row, int column) const
	{
		return m_rows[row].Get(column);
	}

	void Mat44f::Set(int row, int column, float f)
	{
		m_rows[row].Set(column, f);
	}

	void Mat44f::SetIdentity()
	{
		m_rows[0] = Vec4f(1, 0, 0, 0);
		m_rows[1] = Vec4f(0, 1, 0, 0);
		m_rows[2] = Vec4f(0, 0, 1, 0);
		m_rows[3] = Vec4f(0, 0, 0, 1);
	}

	void Mat44f::Transpose()
	{
		Mat44f res;
		for (int ii = 0; ii < 4; ++ii)
		{
			for (int jj = 0; jj < 4; ++jj)
			{
				res.Set(jj, ii, Get(ii, jj));
			}
		}

		*this = res;
	}

	Mat44f Mat44f::operator*(const Mat44f& other)
	{
		//transpose the second matrix to have a list of columns
		Mat44f transposedOther = other;
		transposedOther.Transpose();

		Mat44f result;
		for (int ii = 0; ii < 4; ++ii) //first matrix row
		{
			for (int jj = 0; jj < 4; ++jj) //second matrix column
			{
				float dot = m_rows[ii].Dot(transposedOther.GetRow(jj));
				result.Set(ii, jj, dot);
			}
		}

		return result;
	}
}
