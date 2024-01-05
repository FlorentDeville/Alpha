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

		const void* GetConstDataPointer() const;

	private:
		ModelIndex(int row, int column, void* pData, const AbstractViewModel* pModel);

		int m_row;
		int m_column;
		void* m_pData;
		const AbstractViewModel* m_pModel;
	};
}
