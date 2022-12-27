/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Editors
{
	class BaseModel;

	class ComponentWidget : public Widgets::Widget
	{
	public:
		ComponentWidget();
		~ComponentWidget();

		void Update() override;

		void SetModel(const BaseModel* pModel);

		void Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize) override;
	private:
		const BaseModel* m_pModel;

		bool m_isDirtyWidget;

		void CreateWidgets();
	};
}
