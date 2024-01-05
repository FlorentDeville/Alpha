/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Models/AbstractViewModel.h"

#include "Widgets/Models/Modelndex.h"

namespace Widgets
{
	AbstractViewModel::AbstractViewModel()
	{}

	AbstractViewModel::~AbstractViewModel()
	{}

	ModelIndex AbstractViewModel::CreateIndex(int row, int column, void* pData)
	{
		return ModelIndex(row, column, pData, this);
	}
}
