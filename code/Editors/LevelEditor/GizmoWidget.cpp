/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/GizmoWidget.h"

#include "Rendering/RenderModule.h"

namespace Editors
{
	GizmoWidget::GizmoWidget()
	{
		m_txWs = DirectX::XMMatrixIdentity();
	}

	GizmoWidget::~GizmoWidget()
	{}

	void GizmoWidget::Update()
	{}

	void GizmoWidget::Render()
	{
		RenderRotationManipulator();
	}

	void GizmoWidget::RenderRotationManipulator()
	{
		//rotation x axis
		{
			float rotationAngle = DirectX::XMConvertToRadians(90.f);
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(0, 0, rotationAngle);
			DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(rotation, m_txWs);

			DirectX::XMFLOAT4 red(1, 0, 0, 1);
			Rendering::RenderModule::Get().RenderPrimitiveCircle(mvpMatrix, red);
		}

		//rotation y axis
		{
			DirectX::XMFLOAT4 blue(0, 0, 1, 1);
			Rendering::RenderModule::Get().RenderPrimitiveCircle(m_txWs, blue);
		}

		//rotation z axis
		{
			float rotationAngle = DirectX::XMConvertToRadians(90.f);
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(rotationAngle, 0, 0);
			DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(rotation, m_txWs);

			DirectX::XMFLOAT4 green(0, 1, 0, 1);
			Rendering::RenderModule::Get().RenderPrimitiveCircle(mvpMatrix, green);
		}
	}
}