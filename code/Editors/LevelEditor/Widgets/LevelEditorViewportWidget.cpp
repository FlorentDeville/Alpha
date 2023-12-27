/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Widgets/LevelEditorViewportWidget.h"

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/Widgets/CameraWidget.h"
#include "Editors/LevelEditor/Component.h"
#include "Editors/LevelEditor/Widgets/GizmoModel.h"
#include "Editors/LevelEditor/Widgets/GizmoWidget.h"
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
		, m_isPanning(false)
	{
		m_pCamera = new CameraWidget();
		m_pCamera->OnWsChanged([](const Core::Mat44f& mat) { LevelEditor::Get().SetCameraWs(mat); });

		DirectX::XMVECTOR cameraPosition = DirectX::XMVectorSet(0, 10, -10, 1);
		DirectX::XMVECTOR cameraEulerAngle = DirectX::XMVectorSet(3.14f / 4.f, 0, 0, 0);
		m_pCamera->SetTransform(cameraPosition, cameraEulerAngle);

		m_pGizmoModel = new GizmoModel();
		m_pGizmoWidget = new GizmoWidget();

		m_pGizmoWidget->SetModel(m_pGizmoModel);
	}

	LevelEditorViewportWidget::~LevelEditorViewportWidget()
	{
		delete m_pCamera;
		delete m_pGizmoWidget;
	}

	void LevelEditorViewportWidget::Render()
	{
		m_pCamera->Render(m_aspectRatio);

		const Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();
		const Rendering::Camera* pCamera = renderModule.GetConstCamera();

		//loop through the tree
		const Editors::LevelEditor& levelEditorModule = Editors::LevelEditor::Get();
		const Node* pRoot = levelEditorModule.GetConstLevel().GetConstSceneTree()->GetConstRoot();

		DirectX::XMMATRIX currentWVP = pCamera->GetViewMatrix() * pCamera->GetProjectionMatrix();

		const std::vector<Node*>& children = pRoot->GetConstChildren();
		for (const Node* pNode : children)
		{
			RenderTreeNodeRecursive(pNode, currentWVP, levelEditorModule.m_assetIdToMeshId, levelEditorModule.m_assetIdToMaterialId);
		}

		ClearDepthBuffer();

		m_pGizmoWidget->Render();
	}

	void LevelEditorViewportWidget::Update(uint64_t dt)
	{
		if (!m_enableViewportControl)
			return;

		float dtInSeconds = dt / 1000.f;

		DirectX::XMUINT2 mouseAbsPos;
		Inputs::InputMgr::Get().GetMousePosition(mouseAbsPos.x, mouseAbsPos.y);
		DirectX::XMVECTOR mouse3dPosition = Compute3dPosition(mouseAbsPos);
		Core::Vec4f mouseWs(mouse3dPosition.m128_f32[0], mouse3dPosition.m128_f32[1], mouse3dPosition.m128_f32[2], mouse3dPosition.m128_f32[3]);

		m_pCamera->Update(dtInSeconds);
		m_pGizmoWidget->Update(mouseWs);
	}

	void LevelEditorViewportWidget::SetEnableViewportControl(bool enable)
	{
		m_enableViewportControl = enable;
	}

	GizmoWidget* LevelEditorViewportWidget::GetGizmoWidget()
	{
		return m_pGizmoWidget;
	}

	GizmoModel* LevelEditorViewportWidget::GetGizmoModel()
	{
		return m_pGizmoModel;
	}
}