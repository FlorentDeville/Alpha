/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Models/SelectionRange.h"

#include <cassert>

namespace Widgets
{
	SelectionRange::SelectionRange(const ModelIndex& idx)
		: m_topLeft(idx)
		, m_bottomRight(idx)
	{}

	SelectionRange::SelectionRange(const ModelIndex& topLeft, const ModelIndex& bottomRight)
		: m_topLeft(topLeft)
		, m_bottomRight(bottomRight)
	{
		assert(topLeft.GetParent() == bottomRight.GetParent());
	}

	SelectionRange::~SelectionRange()
	{}

	ModelIndex SelectionRange::GetParent() const
	{
		return m_topLeft.GetParent();
	}

	int SelectionRange::GetTop() const
	{
		return m_topLeft.GetRow();
	}

	int SelectionRange::GetBottom() const
	{
		return m_bottomRight.GetRow();
	}

	int SelectionRange::GetLeft() const
	{
		return m_topLeft.GetColumn();
	}

	int SelectionRange::GetRight() const
	{
		return m_bottomRight.GetColumn();
	}

	bool SelectionRange::IsInside(const ModelIndex& index) const
	{
		if (index.GetParent() != m_topLeft.GetParent())
			return false;

		if (index.GetRow() >= GetTop() && index.GetRow() <= GetBottom() &&
			index.GetColumn() >= GetLeft() && index.GetColumn() <= GetRight())
			return true;

		return false;
	}

	const AbstractViewModel* SelectionRange::GetConstModel() const
	{
		return m_topLeft.GetConstModel();
	}
}
