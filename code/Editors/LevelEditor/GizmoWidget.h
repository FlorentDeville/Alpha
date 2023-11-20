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

		void SetWs(const DirectX::XMMATRIX& txWs);

	private:
		void RenderRotationManipulator();
		void RenderTranslationManipulator();

		void RenderTranslationSingleAxis(const DirectX::XMMATRIX& txWs, const DirectX::XMFLOAT4& color);

		DirectX::XMMATRIX m_txWs;
	};
}
