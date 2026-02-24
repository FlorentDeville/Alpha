/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Models/AbstractViewModel.h"

#include "Widgets/Label.h"
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

	std::string AbstractViewModel::GetHeaderData(int columnIndex)
	{
		return "";
	}

	Widget* AbstractViewModel::GetCellWidget(const ModelIndex& index)
	{
		std::string data = GetData(index);
		Label* pLabel = new Label(data);
		return pLabel;
	}

	SelectionModel* AbstractViewModel::GetSelectionModel()
	{
		return m_pSelectionModel;
	}

	const SelectionModel* AbstractViewModel::GetSelectionModel() const
	{
		return m_pSelectionModel;
	}

	void AbstractViewModel::CommitInsertRows(int start, int count, const ModelIndex& parent)
	{
		m_onCommitInsertRows(start, count, parent);
		m_pSelectionModel->CommitInsertRows(start, count, parent);
	}

	void AbstractViewModel::BeforeRemoveRows(int start, int count, const ModelIndex& parent)
	{
		m_pSelectionModel->BeforeRemoveRows(start, count, parent);
		m_onBeforeRemoveRows(start, count, parent);
	}

	void AbstractViewModel::AfterRemoveRows(int start, int count, const ModelIndex& parent)
	{
		m_onAfterRemoveRows(start, count, parent);
		m_pSelectionModel->AfterRemoveRows(start, count, parent);
	}
}
