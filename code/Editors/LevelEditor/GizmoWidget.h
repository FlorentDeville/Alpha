/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <DirectXMath.h>

namespace Editors
{
	class GizmoModel;
	class Node;

	class GizmoWidget
	{
	public:
		enum ManipulatorMode
		{
			kTranslation,
			kRotation,
			kScale,

			kCount
		};

		GizmoWidget();
		~GizmoWidget();

		void Update();
		void Render();

		void SetModel(GizmoModel* pModel);

		void SetManipulatorMode(ManipulatorMode mode);

	private:
		void RenderRotationManipulator();
		void RenderTranslationManipulator();

		void RenderTranslationSingleAxis(const DirectX::XMMATRIX& txWs, const DirectX::XMFLOAT4& color);

		void OnNodeChanged_Model(Node* pNode);

		GizmoModel* m_pModel;

		DirectX::XMMATRIX m_txWs;

		ManipulatorMode m_manipulatorMode;
	};
}
