/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Matrix/MatrixModel.h"

#include "Core/Math/Mat44f.h"

#include <sstream>

namespace Editors
{
	MatrixModel::MatrixModel(Core::Mat44f* pMatrix)
		: BaseModel()
		, m_pMatrix(pMatrix)
		, m_cache()
		, m_isDirty(true)
	{}

	MatrixModel::~MatrixModel()
	{}

	int MatrixModel::GetRowCount() const
	{
		return 4;
	}

	int MatrixModel::GetColumnCount() const
	{
		return 3; //ignore the last column
	}

	const std::string& MatrixModel::GetData(int rowId, int columnId) const
	{
		if (m_isDirty)
		{
			for (int ii = 0; ii < 4; ++ii)
			{
				for (int jj = 0; jj < 3; ++jj)
				{
					const int BUFFER_SIZE = 9;
					char buffer[BUFFER_SIZE] = { '\0' };
					snprintf(buffer, BUFFER_SIZE, "%.2f", m_pMatrix->Get(ii, jj));
					m_cache[ii][jj] = buffer;
				}
			}

			m_isDirty = false;
		}

		return m_cache[rowId][columnId];
	}

	void MatrixModel::SetData(int rowId, int columnId, const std::string& value)
	{
		//first check if the string is a float
		std::istringstream iss(value);

		float f;
		iss >> f;
		if (!iss.eof() || iss.fail())
		{
			f = 0; // an error occured
		}

		m_pMatrix->Set(rowId, columnId, f);
	}
}
