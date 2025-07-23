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

		ModelIndex CreateIndex(int row, int column, const void* pData) const;

		virtual ModelIndex GetParent(const ModelIndex& child) const = 0;
		virtual ModelIndex GetIndex(int row, int column, const ModelIndex& parent) const = 0;
		virtual int GetRowCount(const ModelIndex& parent) const = 0;
		virtual int GetColumnCount(const ModelIndex& parent) const = 0;

		virtual std::string GetData(const ModelIndex& idx) = 0;
		virtual std::string GetHeaderData(int columnIndex);

		SelectionModel* GetSelectionModel();

		//call this after inserting new rows in the model
		void CommitInsertRows(int start, int count, const ModelIndex& parent);

		//call this before removing the rows from the model
		void BeforeRemoveRows(int start, int count, const ModelIndex& parent);

		//call this after the rows were removed from the model
		void AfterRemoveRows(int start, int count, const ModelIndex& parent);

		//Event called when the model has changed a value and the view needs to be refreshed.
		EVENT_DECL(DataChanged, void(const ModelIndex& index))

	private:
		SelectionModel* m_pSelectionModel;

		//Used only by widgets. they need to be friend with this class.
		PRIVATE_EVENT_DECL(CommitInsertRows, void(int start, int count, const ModelIndex& parent))
		PRIVATE_EVENT_DECL(BeforeRemoveRows, void(int start, int count, const ModelIndex& parent))
		PRIVATE_EVENT_DECL(AfterRemoveRows, void(int start, int count, const ModelIndex& parent))
	};
}
