/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Models/Modelndex.h"

namespace Widgets
{
	ModelIndex::ModelIndex()
		: m_row(-1)
		, m_column(-1)
		, m_pData(nullptr)
		, m_pModel(nullptr)
	{}

	ModelIndex::~ModelIndex()
	{}

	bool ModelIndex::IsValid() const
	{
		if (m_row == -1 || m_column == -1)
			return false;

		return true;
	}

	int ModelIndex::GetRow() const
	{
		return m_row;
	}

	int ModelIndex::GetColumn() const
	{
		return m_column;
	}

	const void* ModelIndex::GetConstDataPointer() const
	{
		return m_pData;
	}

	ModelIndex::ModelIndex(int row, int column, void* pData, const AbstractViewModel* pModel)
		: m_row(row)
		, m_column(column)
		, m_pData(pData)
		, m_pModel(pModel)
	{}
}
