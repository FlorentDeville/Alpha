/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Rendering/Texture/TextureId.h"

#include "Widgets/Widget.h"

namespace Widgets
{
	class Icon;
}

namespace Editors
{
	class EntityModel;
	class ComponentWidget;

	class EntityWidget : public Widgets::Widget
	{
	public:
		EntityWidget();
		~EntityWidget();

		void Update(uint64_t dt) override;

		void SetModel(EntityModel* pModel);

	private:
		EntityModel* m_pModel;

		Rendering::TextureId m_collapsedIcon;
		Rendering::TextureId m_expandedIcon;

		bool m_isDirtyWidget;

		void CreateWidgets();

		void OnMouseDown_ExpandIcon(const Widgets::MouseEvent& ev, Widgets::Icon* pIcon, ComponentWidget* pComponentWidget);
	};
}
