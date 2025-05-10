/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Widgets
{
	class AbstractViewModel;
	class ModelIndex
	{
		friend AbstractViewModel;

	public:
		ModelIndex();
		~ModelIndex();

		bool IsValid() const;

		int GetRow() const;
		int GetColumn() const;

		ModelIndex GetParent() const;
		ModelIndex GetIndex(int row, int column) const;

		ModelIndex GetSiblingAtColumn(int column) const;

		const void* GetConstDataPointer() const;

		const AbstractViewModel* GetConstModel() const;

		bool operator==(const ModelIndex& other) const;
		bool operator!=(const ModelIndex& other) const;

	private:
		ModelIndex(int row, int column, const void* pData, const AbstractViewModel* pModel);

		int m_row;
		int m_column;
		const void* m_pData;
		const AbstractViewModel* m_pModel;
	};
}
