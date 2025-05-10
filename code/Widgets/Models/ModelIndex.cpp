/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Models/ModelIndex.h"

#include "Widgets/Models/AbstractViewModel.h"

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

	ModelIndex ModelIndex::GetParent() const
	{
		return m_pModel->GetParent(*this);
	}

	ModelIndex ModelIndex::GetIndex(int row, int column) const
	{
		return m_pModel->GetIndex(row, column, *this);
	}

	ModelIndex ModelIndex::GetSiblingAtColumn(int column) const
	{
		return m_pModel->GetIndex(m_row, column, GetParent());
	}

	const void* ModelIndex::GetConstDataPointer() const
	{
		return m_pData;
	}

	const AbstractViewModel* ModelIndex::GetConstModel() const
	{
		return m_pModel;
	}

	bool ModelIndex::operator==(const ModelIndex& other) const
	{
		if (m_row == other.m_row && m_column == other.m_column && m_pData == other.m_pData && m_pModel == other.m_pModel)
			return true;

		return false;
	}

	bool ModelIndex::operator!=(const ModelIndex& other) const
	{
		return !(*this == other);
	}

	ModelIndex::ModelIndex(int row, int column, const void* pData, const AbstractViewModel* pModel)
		: m_row(row)
		, m_column(column)
		, m_pData(pData)
		, m_pModel(pModel)
	{}
}
