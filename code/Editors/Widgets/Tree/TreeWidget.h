/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include "Rendering/Texture/TextureId.h"

namespace Widgets
{
	class Button;
	class Layout;
}

namespace Editors
{
	class BaseModel;

	class TreeWidget : public Widgets::Widget
	{
	public:
		using OnItemClickedCallback = std::function<bool(BaseModel* pModel, int rowId)>;

		TreeWidget();
		~TreeWidget();

		void SetModel(BaseModel* pModel);

		void Update() override;

		void OnItemClicked(const OnItemClickedCallback& callback);

	private:
		BaseModel* m_pModel;

		bool m_isDirtyWidget;

		Rendering::TextureId m_expandedIcon;
		Rendering::TextureId m_collapsedIcon;

		OnItemClickedCallback m_onItemClicked;

		Widgets::Button* m_pSelectedButton;

		void CreateWidgets();

		void CreateRecursiveWidgets(Widgets::Layout* pParentLayout, BaseModel* pModel, int offsetX);
	};
}
