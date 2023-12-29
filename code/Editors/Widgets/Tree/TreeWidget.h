/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include "Rendering/Texture/TextureId.h"

namespace Widgets
{
	class Button;
	class Icon;
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

		void Update(uint64_t dt) override;

		void OnItemClicked(const OnItemClickedCallback& callback);

		BaseModel* GetSelectedItemModel();

	private:
		BaseModel* m_pModel;

		bool m_isDirtyWidget;

		Rendering::TextureId m_expandedIcon;
		Rendering::TextureId m_collapsedIcon;

		OnItemClickedCallback m_onItemClicked;

		Widgets::Button* m_pSelectedButton;
		BaseModel* m_pSelectedItemModel;

		void CreateWidgets();

		void CreateRecursiveWidgets(Widgets::Layout* pParentLayout, BaseModel* pModel, int offsetX);

		void OnMouseDown_CollapseIcon(const Widgets::MouseEvent& ev, Widgets::Layout* pChildrenLayout, Widgets::Icon* pIcon);
	};
}
