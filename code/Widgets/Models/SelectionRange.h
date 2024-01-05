/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Models/ModelIndex.h"

namespace Widgets
{
	class SelectionRange
	{
	public:
		SelectionRange(const ModelIndex& idx);
		SelectionRange(const ModelIndex& topLeft, const ModelIndex& bottomRight);
		~SelectionRange();

		ModelIndex GetParent() const;

		int GetTop() const;
		int GetBottom() const;
		int GetLeft() const;
		int GetRight() const;

		bool IsInside(const ModelIndex& index) const;

		const AbstractViewModel* GetConstModel() const;

	private:
		ModelIndex m_topLeft;
		ModelIndex m_bottomRight;
	};
}
