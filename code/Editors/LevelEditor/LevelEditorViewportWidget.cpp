/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/LevelEditorViewportWidget.h"

#include "Core/Math/Mat44f.h"

#include "Editors/LevelEditor/Component.h"
#include "Editors/LevelEditor/GizmoWidget.h"
#include "Editors/LevelEditor/LevelEditor.h"
#include "Editors/LevelEditor/SceneTree/Entity.h"
#include "Editors/LevelEditor/SceneTree/Node.h"
#include "Editors/LevelEditor/SceneTree/SceneTree.h"

#include "Inputs/InputMgr.h"

#include "Rendering/Camera.h"
#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"

//needed for the operator* between vectors and floats.
using namespace DirectX;

//#pragma optimize("", off)

namespace Editors
{
	void RenderTreeNodeRecursive(const Node* pNode, const DirectX::XMMATRIX& parentWVP, const std::map<Systems::AssetId, Rendering::MeshId>& assetIdToMeshId,
		const std::map<Systems::AssetId, Rendering::MaterialId>& assetIdToMaterialId)
	{
		DirectX::XMMATRIX currentWVP = parentWVP;

		const Entity* pEntity = pNode->ToConstEntity();
		if (!pEntity)
			return;

		const Component* pTransformComponent = pEntity->GetComponent("Transform");
		if (pTransformComponent)
		{
			Core::Mat44f local;
			pTransformComponent->GetPropertyValue("Local", local);

			DirectX::XMMATRIX dxLocal(
				local.GetX().GetX(), local.GetX().GetY(), local.GetX().GetZ(), local.GetX().GetW(),
				local.GetY().GetX(), local.GetY().GetY(), local.GetY().GetZ(), local.GetY().GetW(),
				local.GetZ().GetX(), local.GetZ().GetY(), local.GetZ().GetZ(), local.GetZ().GetW(),
				local.GetT().GetX(), local.GetT().GetY(), local.GetT().GetZ(), local.GetT().GetW()
			);

			currentWVP = dxLocal * currentWVP;
		}

		const Component* pRenderingComponent = pEntity->GetComponent("Rendering");
		if (pRenderingComponent)
		{
			Systems::AssetId meshAssetId;
			Systems::AssetId materialAssetId;
			pRenderingComponent->GetPropertyValue("Mesh", meshAssetId);
			pRenderingComponent->GetPropertyValue("Material", materialAssetId);

			if (meshAssetId != Systems::AssetId::INVALID && materialAssetId != Systems::AssetId::INVALID)
			{
				Rendering::MeshId meshId = assetIdToMeshId.find(meshAssetId)->second;
				Rendering::MaterialId materialId = assetIdToMaterialId.find(materialAssetId)->second;

				Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

				const Rendering::Material* pMaterial = Rendering::MaterialMgr::Get().GetMaterial(materialId);
				renderer.BindMaterial(*pMaterial, currentWVP);

				const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(meshId);
				renderer.RenderMesh(*pMesh);
			}
		}

		//recursive call to children
		const std::vector<Node*>& children = pNode->GetConstChildren();
		for (const Node* pChild : children)
		{
			RenderTreeNodeRecursive(pChild, currentWVP, assetIdToMeshId, assetIdToMaterialId);
		}
	}

	LevelEditorViewportWidget::LevelEditorViewportWidget(int width, int height)
		: Widgets::Viewport_v2(width, height)
		, m_enableViewportControl(false)
		, m_firstFrameMouseDown(true)
		, m_mousePreviousPos()
		, m_isPanning(false)
		, m_cameraState(CameraState::None)
		, m_translationSpeed(10.f)
		, m_rotationSpeed(0.3f)
	{
		m_cameraPosition = DirectX::XMVectorSet(0, 10, -10, 1);
		m_cameraEulerAngle = DirectX::XMVectorSet(3.14f / 4.f, 0, 0, 0);

		DirectX::XMMATRIX YRotation = DirectX::XMMatrixRotationY(m_cameraEulerAngle.m128_f32[1]);
		DirectX::XMMATRIX XRotation = DirectX::XMMatrixRotationX(m_cameraEulerAngle.m128_f32[0]);
		m_cameraRotation = XRotation * YRotation;

		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(m_cameraPosition);
		m_cameraTransform = m_cameraRotation * translationMatrix;

		m_pGizmoWidget = new GizmoWidget();
		m_pGizmoWidget->SetWs(DirectX::XMMatrixTranslation(0, 2, 0));
	}

	LevelEditorViewportWidget::~LevelEditorViewportWidget()
	{}

	void LevelEditorViewportWidget::Render()
	{
		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();
		Rendering::Camera* pCamera = renderModule.GetCamera();

		DirectX::XMVECTOR cameraUp = DirectX::XMVectorSet(0, 1, 0, 1);
		DirectX::XMVECTOR targetOffset = DirectX::XMVectorSet(0, 0, 1, 1);
		DirectX::XMVECTOR cameraLookAt = DirectX::XMVector4Transform(targetOffset, m_cameraTransform);
		DirectX::XMVECTOR cameraPosition = m_cameraTransform.r[3];
		pCamera->SetLookAt(cameraPosition, cameraLookAt, cameraUp);

		constexpr float fovRad = DirectX::XMConvertToRadians(45.f);
		const float nearDistance = 0.1f;
		const float farDistance = 1000.f;
		pCamera->SetProjection(fovRad, m_aspectRatio, nearDistance, farDistance);

		//loop through the tree
		const Editors::LevelEditor& levelEditorModule = Editors::LevelEditor::Get();
		const Node* pRoot = levelEditorModule.GetConstLevel().GetConstSceneTree()->GetConstRoot();

		DirectX::XMMATRIX currentWVP = pCamera->GetViewMatrix() * pCamera->GetProjectionMatrix();

		const std::vector<Node*>& children = pRoot->GetConstChildren();
		for (const Node* pNode : children)
		{
			RenderTreeNodeRecursive(pNode, currentWVP, levelEditorModule.m_assetIdToMeshId, levelEditorModule.m_assetIdToMaterialId);
		}

		m_pGizmoWidget->Render();
	}

	void LevelEditorViewportWidget::Update(uint64_t dt)
	{
		if (!m_enableViewportControl)
			return;

		float dtInSeconds = dt / 1000.f;

		UpdateCamera(dtInSeconds);

		//const float TRANSLATION_SPEED = 0.5f;
		//const float PAN_SPEED = 0.1f;

		//if (inputs.IsMouseMiddleButtonDown())
		//{
		//	OutputDebugString("pan\n");
		//	DirectX::XMUINT2 mousePosition;
		//	inputs.GetMousePosition(mousePosition.x, mousePosition.y);
		//	if (!m_isPanning)
		//	{
		//		m_mousePreviousPos = mousePosition;
		//		m_isPanning = true;
		//	}

		//	DirectX::XMINT2 delta;
		//	delta.x = m_mousePreviousPos.x - mousePosition.x;
		//	delta.y = m_mousePreviousPos.y - mousePosition.y;

		//	if (delta.x != 0)
		//	{
		//		DirectX::XMVECTOR xAxis = m_cameraRotation.r[0];
		//		m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, xAxis * static_cast<float>(delta.x) * PAN_SPEED);
		//		updateCameraTransform = true;
		//	}
		//	if (delta.y != 0)
		//	{
		//		DirectX::XMVECTOR yAxis = m_cameraRotation.r[1];
		//		m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, yAxis * static_cast<float>(-delta.y) * PAN_SPEED);
		//		updateCameraTransform = true;
		//	}

		//	m_mousePreviousPos = mousePosition;
		//}
		//else if (m_isPanning)
		//{
		//	m_isPanning = false;
		//}

		//int16_t mouseWheelDistance = inputs.GetMouseWheelDistance();
		//if (mouseWheelDistance != 0)
		//{
		//	const float CAMERA_DISTANCE_SPEED = 0.05f;
		//	const float MIN_DISTANCE = 2;
		//	DirectX::XMVECTOR zAxis = m_cameraRotation.r[2];
		//	m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, zAxis * mouseWheelDistance * CAMERA_DISTANCE_SPEED);
		//	updateCameraTransform = true;
		//}
	}

	void LevelEditorViewportWidget::SetEnableViewportControl(bool enable)
	{
		m_enableViewportControl = enable;
	}

	void LevelEditorViewportWidget::UpdateCamera(float dtInSeconds)
	{
		switch (m_cameraState)
		{
		case Editors::LevelEditorViewportWidget::None:
			UpdateCamera_None(dtInSeconds);
			break;

		case Editors::LevelEditorViewportWidget::FPS:
			UpdateCamera_FPS(dtInSeconds);
			break;

		default:
			assert(false);
			break;
		}
	}

	void LevelEditorViewportWidget::UpdateCamera_None(float dtInSeconds)
	{
		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
		if (inputs.IsMouseRightButtonDown())
			m_cameraState = CameraState::FPS;
	}

	void LevelEditorViewportWidget::UpdateCamera_FPS(float dtInSeconds)
	{
		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
		if (!inputs.IsMouseRightButtonDown())
		{
			m_firstFrameMouseDown = true;
			m_cameraState = CameraState::None;
			return;
		}

		//speed up/down
		int16_t mouseWheelDistance = inputs.GetMouseWheelDistance();
		if (mouseWheelDistance != 0)
		{
			m_translationSpeed += mouseWheelDistance * 0.05f;
			//m_rotationSpeed += mouseWheelDistance * 0.0001f;

			if (m_translationSpeed < 1) m_translationSpeed = 1.f;
			//if (m_rotationSpeed < 0.003f) m_rotationSpeed = 0.003f;
		}

		bool updateCameraTransform = false;

		if (inputs.IsKeyPressed('W')) //forward
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[2];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('S')) //backward
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[2];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, -zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('A')) //left
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[0];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, -zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
			printf("key D pressed");
		}
		if (inputs.IsKeyPressed('D')) //right
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[0];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
			printf("key D pressed");
		}
		if (inputs.IsKeyPressed('Q')) //up
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[1];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('E')) //down
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[1];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, -zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
		}

		//camera rotation
		{
			DirectX::XMUINT2 mousePosition;
			inputs.GetMousePosition(mousePosition.x, mousePosition.y);
			if (m_firstFrameMouseDown)
			{
				m_mousePreviousPos = mousePosition;
				m_firstFrameMouseDown = false;
			}

			DirectX::XMINT2 delta;
			delta.x = m_mousePreviousPos.x - mousePosition.x;
			delta.y = m_mousePreviousPos.y - mousePosition.y;

			if (delta.x != 0 || delta.y != 0)
			{
				DirectX::XMVECTOR eulerRotation = DirectX::XMVectorSet(-static_cast<float>(delta.y) * m_rotationSpeed * dtInSeconds, -static_cast<float>(delta.x) * m_rotationSpeed * dtInSeconds, 0, 0);
				m_cameraEulerAngle += eulerRotation;

				DirectX::XMMATRIX YRotation = DirectX::XMMatrixRotationY(m_cameraEulerAngle.m128_f32[1]);
				DirectX::XMMATRIX XRotation = DirectX::XMMatrixRotationX(m_cameraEulerAngle.m128_f32[0]);

				m_cameraRotation = DirectX::XMMatrixMultiply(XRotation, YRotation);
				m_mousePreviousPos = mousePosition;
				updateCameraTransform = true;
			}
		}

		if (updateCameraTransform)
		{
			DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(m_cameraPosition);
			m_cameraTransform = DirectX::XMMatrixMultiply(m_cameraRotation, translationMatrix);
		}
	}
}