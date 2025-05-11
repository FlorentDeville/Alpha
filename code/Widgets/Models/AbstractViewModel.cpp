/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Models/AbstractViewModel.h"

#include "Widgets/Models/ModelIndex.h"
#include "Widgets/Models/SelectionModel.h"

namespace Widgets
{
	AbstractViewModel::AbstractViewModel()
		: m_pSelectionModel(new SelectionModel())
	{}

	AbstractViewModel::~AbstractViewModel()
	{
		delete m_pSelectionModel;
		m_pSelectionModel = nullptr;
	}

	ModelIndex AbstractViewModel::CreateIndex(int row, int column, const void* pData) const
	{
		return ModelIndex(row, column, pData, this);
	}

	SelectionModel* AbstractViewModel::GetSelectionModel()
	{
		return m_pSelectionModel;
	}

	void AbstractViewModel::CommitInsertRows(int start, int count, const ModelIndex& parent)
	{
		m_onCommitInsertRows(start, count, parent);
		m_pSelectionModel->CommitInsertRows(start, count, parent);
	}

	void AbstractViewModel::RemoveRows(int start, int count, const ModelIndex& parent)
	{
		m_onRemoveRows(start, count, parent);
		m_pSelectionModel->RemoveRows(start, count, parent);
	}
}
