/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"

#include <string>

namespace Widgets
{
	class ModelIndex;
	class SelectionModel;

	class AbstractViewModel
	{
		friend class TableView;

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

		//call this after inserting new rows in the model
		void CommitInsertRows(int start, int count, const ModelIndex& parent);

	private:
		SelectionModel* m_pSelectionModel;

		//Used only by widgets. they need to be friend with this class.
		PRIVATE_EVENT_DECL(CommitInsertRows, void(int start, int count, const ModelIndex& parent))
	};
}
