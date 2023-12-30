/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"
#include "Widgets/Widget.h"

namespace Widgets
{
	class Layout;
	class TextBox;
}

namespace Editors
{
	class BaseModel;

	class MatrixWidget : public Widgets::Widget
	{
	public:
		MatrixWidget();
		~MatrixWidget();

		void SetValue(const Core::Mat44f& value);

		EVENT_DECL(ValueChanged, void(const Core::Mat44f&))
	private:
		Core::Mat44f m_cachedMatrix;

		Widgets::Layout* m_pLayout;
		Widgets::TextBox* m_pTextBox[4][3];
	};
}
