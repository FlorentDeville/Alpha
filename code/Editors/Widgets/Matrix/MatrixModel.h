/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/BaseModel.h"

namespace Core
{
	class Mat44f;
}
namespace Editors
{
	class MatrixModel : public BaseModel
	{
	public:
		MatrixModel(Core::Mat44f* pMatrix);
		~MatrixModel();

		int GetRowCount() const override;
		int GetColumnCount() const override;

		const std::string& GetData(int rowId = 0, int columnId = 0) const override;

		void SetData(int rowId, int columnId, const std::string& value) override;

	private:
		Core::Mat44f* m_pMatrix;

		mutable std::string m_cache[4][3];
		mutable bool m_isDirty;
	};
}