/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/GizmoWidget.h"

#include "Editors/LevelEditor/LevelEditor.h"
#include "Editors/LevelEditor/GizmoModel.h"

#include "Rendering/RenderModule.h"

using namespace DirectX;

//#pragma optimize("", off)

namespace Editors
{
	GizmoWidget::GizmoWidget()
		: m_manipulatorMode(kTranslation)
		, m_pModel(nullptr)
		, BASE_DIAMETER(0.075f)
		, LENGTH(7.f)
		, CONE_BASE_DIAMETER(0.5)
		, CONE_BASE_LENGTH(0.75)
		, ROTATION_DIAMATER(5.f)
		, SCALE_SQUARE_SIZE(0.5f)
	{
		m_txWs = DirectX::XMMatrixIdentity();
	}

	GizmoWidget::~GizmoWidget()
	{
		if (m_pModel)
		{
			delete m_pModel;
			m_pModel = nullptr;
		}
	}

	void GizmoWidget::Update()
	{}

	void GizmoWidget::Render()
	{
		switch (m_manipulatorMode)
		{
		case kTranslation:
			RenderTranslationManipulator();
			break;

		case kRotation:
			RenderRotationManipulator();
			break;

		case kScale:
			RenderScaleManipulator();
			break;

		default:
			break;
		}
	}

	void GizmoWidget::SetModel(GizmoModel* pModel)
	{
		if (m_pModel == pModel)
			return;

		if (m_pModel)
			delete m_pModel;

		m_pModel = pModel;

		if (!pModel)
			return;

		pModel->OnNodeChanged([this](Node* pNode) { OnNodeChanged_Model(pNode); });
		m_txWs = pModel->GetTransform();
	}

	void GizmoWidget::SetManipulatorMode(ManipulatorMode mode)
	{
		m_manipulatorMode = mode;
	}

	void GizmoWidget::RenderRotationManipulator()
	{
		Core::Vec4f objectPosition(m_txWs.r[0].m128_f32[0], m_txWs.r[0].m128_f32[1], m_txWs.r[0].m128_f32[2], 1);
		float size = ComputeConstantScreenSizeScale(objectPosition);

		float realRotationDiameter = ROTATION_DIAMATER * size;
		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(realRotationDiameter, realRotationDiameter, realRotationDiameter);

		//rotation x axis
		{
			constexpr float rotationAngle = DirectX::XMConvertToRadians(90.f);
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(0, 0, rotationAngle);
			DirectX::XMMATRIX mvpMatrix = scale * rotation * m_txWs;

			DirectX::XMFLOAT4 red(1, 0, 0, 1);
			Rendering::RenderModule::Get().RenderPrimitiveTorus(mvpMatrix, red);
		}

		//rotation y axis
		{
			DirectX::XMFLOAT4 green(0, 1, 0, 1);

			DirectX::XMMATRIX mvpMatrix = scale * m_txWs;
			Rendering::RenderModule::Get().RenderPrimitiveTorus(mvpMatrix, green);
		}

		//rotation z axis
		{
			constexpr float rotationAngle = DirectX::XMConvertToRadians(90.f);
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(rotationAngle, 0, 0);
			DirectX::XMMATRIX mvpMatrix = scale * rotation * m_txWs;

			DirectX::XMFLOAT4 blue(0, 0, 1, 1);
			Rendering::RenderModule::Get().RenderPrimitiveTorus(mvpMatrix, blue);
		}
	}

	void GizmoWidget::RenderTranslationManipulator()
	{
		Rendering::RenderModule& renderingMgr = Rendering::RenderModule::Get();

		//x axis
		{
			//rotate everything 90 degres around z axis
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(-DirectX::XM_PIDIV2);
			DirectX::XMFLOAT4 red(1, 0, 0, 0);
			RenderTranslationSingleAxis(rotation * m_txWs, red);
		}

		//y axis
		{
			DirectX::XMFLOAT4 green(0, 1, 0, 0);
			RenderTranslationSingleAxis(m_txWs, green);
		}

		//z axis
		{
			//rotate everything 90 degres around z axis
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationX(DirectX::XM_PIDIV2);
			DirectX::XMFLOAT4 blue(0, 0, 1, 0);
			RenderTranslationSingleAxis(rotation * m_txWs, blue);
		}
	}

	void GizmoWidget::RenderScaleManipulator()
	{
		Rendering::RenderModule& renderingMgr = Rendering::RenderModule::Get();

		//x axis
		{
			//rotate everything 90 degres around z axis
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(-DirectX::XM_PIDIV2);
			DirectX::XMFLOAT4 red(1, 0, 0, 0);
			RenderScaleSingleAxis(rotation * m_txWs, red);
		}

		//y axis
		{
			DirectX::XMFLOAT4 green(0, 1, 0, 0);
			RenderScaleSingleAxis(m_txWs, green);
		}

		//z axis
		{
			//rotate everything 90 degres around z axis
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationX(DirectX::XM_PIDIV2);
			DirectX::XMFLOAT4 blue(0, 0, 1, 0);
			RenderScaleSingleAxis(rotation * m_txWs, blue);
		}
	}

	void GizmoWidget::RenderTranslationSingleAxis(const DirectX::XMMATRIX& txWs, const DirectX::XMFLOAT4& color)
	{
		Rendering::RenderModule& renderingMgr = Rendering::RenderModule::Get();
		
		Core::Vec4f objectPosition(txWs.r[0].m128_f32[0], txWs.r[0].m128_f32[1], txWs.r[0].m128_f32[2], 1);
		float size = ComputeConstantScreenSizeScale(objectPosition);

		float realDiameter = BASE_DIAMETER * size;
		float realLength = LENGTH * size;

		{
			DirectX::XMVECTOR scale = DirectX::XMVectorSet(realDiameter, realLength, realDiameter, 0);
			DirectX::XMVECTOR translation = DirectX::XMVectorSet(0.f, realLength * 0.5f, 0, 0);
			DirectX::XMMATRIX txLs = DirectX::XMMatrixAffineTransformation(scale, DirectX::g_XMZero, DirectX::g_XMIdentityR3, translation);
			DirectX::XMMATRIX ws = txLs * txWs;
			renderingMgr.RenderPrimitiveCylinder(ws, color);
		}

		{
			
			float coneDiameter = CONE_BASE_DIAMETER * size;
			float coneLength = CONE_BASE_LENGTH * size;

			DirectX::XMVECTOR localTranslation = DirectX::XMVectorSet(0, realLength, 0, 0);
			DirectX::XMVECTOR localRotationOrigin = DirectX::XMVectorSet(0, 0, 0, 1);
			DirectX::XMVECTOR localRotation = DirectX::XMQuaternionIdentity();
			DirectX::XMVECTOR localScale = DirectX::XMVectorSet(coneDiameter, coneLength, coneDiameter, 0);
			DirectX::XMMATRIX txLs = DirectX::XMMatrixAffineTransformation(localScale, localRotationOrigin, localRotation, localTranslation);

			DirectX::XMMATRIX ws = txLs * txWs;
			renderingMgr.RenderPrimitiveCone(ws, color);
		}
	}

	void GizmoWidget::RenderScaleSingleAxis(const DirectX::XMMATRIX& txWs, const DirectX::XMFLOAT4& color) const
	{
		Rendering::RenderModule& renderingMgr = Rendering::RenderModule::Get();

		Core::Vec4f objectPosition(txWs.r[0].m128_f32[0], txWs.r[0].m128_f32[1], txWs.r[0].m128_f32[2], 1);
		float size = ComputeConstantScreenSizeScale(objectPosition);

		float realDiameter = BASE_DIAMETER * size;
		float realLength = LENGTH * size;

		{
			DirectX::XMVECTOR scale = DirectX::XMVectorSet(realDiameter, realLength, realDiameter, 0);
			DirectX::XMVECTOR translation = DirectX::XMVectorSet(0.f, realLength * 0.5f, 0, 0);
			DirectX::XMMATRIX txLs = DirectX::XMMatrixAffineTransformation(scale, DirectX::g_XMZero, DirectX::g_XMIdentityR3, translation);
			DirectX::XMMATRIX ws = txLs * txWs;
			renderingMgr.RenderPrimitiveCylinder(ws, color);
		}

		{

			float coneDiameter = CONE_BASE_DIAMETER * size;
			float coneLength = SCALE_SQUARE_SIZE * size;

			DirectX::XMVECTOR localTranslation = DirectX::XMVectorSet(0, realLength, 0, 0);
			DirectX::XMVECTOR localRotationOrigin = DirectX::XMVectorSet(0, 0, 0, 1);
			DirectX::XMVECTOR localRotation = DirectX::XMQuaternionIdentity();
			DirectX::XMVECTOR localScale = DirectX::XMVectorSet(coneLength, coneLength, coneLength, 0);
			DirectX::XMMATRIX txLs = DirectX::XMMatrixAffineTransformation(localScale, localRotationOrigin, localRotation, localTranslation);

			DirectX::XMMATRIX ws = txLs * txWs;
			renderingMgr.RenderPrimitiveCube(ws, color);
		}
	}

	float GizmoWidget::ComputeConstantScreenSizeScale(const Core::Vec4f& objectPosition) const
	{
		const LevelEditor& levelEditor = LevelEditor::Get();
		const Core::Mat44f& camera = levelEditor.GetCameraWs();
		float fov = levelEditor.GetFovRad();

		Core::Vec4f dt = camera.GetT() - objectPosition;
		float distance = dt.Length();
		float worldSize = (2 * tanf(fov * 0.5f)) * distance;
		const float SCREEN_RATIO = 0.025f;
		float size = SCREEN_RATIO * worldSize;
		return size;
	}

	void GizmoWidget::OnNodeChanged_Model(Node* pNode)
	{
		m_txWs = m_pModel->GetTransform();
	}
}