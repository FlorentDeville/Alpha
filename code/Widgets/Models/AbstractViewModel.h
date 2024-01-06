/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackList.h"
#include "Core/Callbacks/CallbackMacro.h"

#include <string>

namespace Widgets
{
	class ModelIndex;
	class SelectionModel;

	class AbstractViewModel
	{
	public:
		AbstractViewModel();
		virtual ~AbstractViewModel();

		ModelIndex CreateIndex(int row, int column, void* pData) const;

		virtual ModelIndex GetParent(const ModelIndex& child) const = 0;
		virtual ModelIndex GetIndex(int row, int column, const ModelIndex& parent) const = 0;
		virtual int GetRowCount(const ModelIndex& parent) = 0;
		virtual int GetColumnCount(const ModelIndex& parent) const = 0;

		virtual std::string GetData(const ModelIndex& idx) = 0;

		SelectionModel* GetSelectionModel();

		//call this event after inserting new rows in the model
		EVENT_DECL(CommitInsertRows, void(int start, int count, const ModelIndex& parent))

	private:
		SelectionModel* m_pSelectionModel;
	};
}
