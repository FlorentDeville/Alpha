/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/AssetId/AssetIdWidget.h"

#include "Editors/Widgets/AssetId/AssetIdModel.h"
#include "Systems/Assets/AssetId.h"

namespace Editors
{
	AssetIdWidget::AssetIdWidget(int32_t locX, int32_t locY, float scale)
		: Label()
		, m_pModel(nullptr)
	{
		SetX(locX);
		SetY(locY);
		SetFontScale(scale);
	}

	AssetIdWidget::~AssetIdWidget()
	{}

	void AssetIdWidget::SetModel(const AssetIdModel* pModel)
	{
		m_pModel = pModel;
	}

	void AssetIdWidget::Update()
	{
		SetText(m_pModel->GetData());
	}
}
