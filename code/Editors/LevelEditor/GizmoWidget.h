/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <DirectXMath.h>

namespace Editors
{
	class GizmoWidget
	{
	public:
		GizmoWidget();
		~GizmoWidget();

		void Update();
		void Render();

	private:
		void RenderRotationManipulator();

		DirectX::XMMATRIX m_txWs;
	};
}
