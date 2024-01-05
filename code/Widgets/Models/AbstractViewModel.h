/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

namespace Widgets
{
	class ModelIndex;

	class AbstractViewModel
	{
	public:
		AbstractViewModel();
		virtual ~AbstractViewModel();

		ModelIndex CreateIndex(int row, int column, void* pData);

		virtual ModelIndex GetIndex(int row, int column, const ModelIndex& parent) = 0;
		virtual int GetRowCount(const ModelIndex& parent) = 0;
		virtual int GetColumnCount(const ModelIndex& parent) = 0;

		virtual std::string GetData(const ModelIndex& idx) = 0;
	};
}
