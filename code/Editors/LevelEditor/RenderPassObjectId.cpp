/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/LevelEditor/RenderPassObjectId.h"

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

#include "Rendering/RenderModule.h"
#include "Rendering/PipelineState/PipelineStateDesc.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shaders/ShaderMgr.h"

#include "Systems/Rendering/Renderable/RenderableLight.h"
#include "Systems/Rendering/Renderable/RenderableObject.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"
#include "Systems/Objects/GameObject.h"

namespace Editors
{
	struct ConstBufferPerFrame
	{
		Core::Mat44f m_view;
		Core::Mat44f m_proj;
	};

	struct ConstBufferPerObject
	{
		Core::Mat44f m_world;
		uint32_t m_objectId[4];
	};

	RenderPassObjectId::RenderPassObjectId(int32_t width, int32_t height)
		: IRenderPass()
	{
		m_pObjectIdRenderTarget = new Rendering::RenderTarget(width, height, DXGI_FORMAT_R8G8B8A8_UINT, Core::Vec4f(0, 0, 0, 0));

		m_pObjectIdRootSig = Rendering::RootSignatureMgr::Get().GetRootSignature(Rendering::EngineRootSigs::OBJECTID);
		Rendering::Shader* pVS = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::OBJECTID_VS);
		Rendering::Shader* pPS = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::OBJECTID_PS);

		Rendering::PipelineStateDesc psoDesc;
		psoDesc.m_pRs = m_pObjectIdRootSig;
		psoDesc.m_pVs = pVS;
		psoDesc.m_pPs = pPS;
		psoDesc.m_depthFunction = Rendering::DepthComparisonMode::Less;
		psoDesc.m_cullMode = Rendering::CullMode::Back;
		psoDesc.m_rtvFormat = Rendering::ResourceFormat::R8G8B8A8_UINT;

		m_pObjectIdPso = new Rendering::PipelineState();
		m_pObjectIdPso->Init_Generic(psoDesc);
	}

	RenderPassObjectId::~RenderPassObjectId()
	{
		delete m_pObjectIdRenderTarget;
		delete m_pObjectIdPso;
	}

	void RenderPassObjectId::PreRender(const Systems::RenderableScene& scene)
	{
		m_pObjectIdRenderTarget->BeginScene();
	}

	void RenderPassObjectId::Render(const Systems::RenderableScene& scene)
	{
		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();

		ConstBufferPerFrame perFrame;
		perFrame.m_view = scene.m_camera.m_view;
		perFrame.m_proj = scene.m_camera.m_proj;

		renderModule.BindMaterial(*m_pObjectIdPso, *m_pObjectIdRootSig);

		renderModule.SetConstantBuffer(1, sizeof(ConstBufferPerFrame), &perFrame, 0);

		uint32_t objectIdCounter = 0;

		for (const Systems::RenderableObject& renderable : scene.m_objects)
		{
			if (!(renderable.m_view & Systems::RenderView::ObjectId))
				continue;

			++objectIdCounter;

			const Core::Mat44f& world = renderable.m_worldTx;
			ConstBufferPerObject perObject;
			perObject.m_world = world;
			perObject.m_objectId[3] = (objectIdCounter & 0xFF000000) >> 24;
			perObject.m_objectId[2] = (objectIdCounter & 0x00FF0000) >> 16;
			perObject.m_objectId[1] = (objectIdCounter & 0x0000FF00) >> 8;
			perObject.m_objectId[0] = (objectIdCounter & 0x000000FF);

			m_objectIdToGuid[objectIdCounter] = renderable.m_pOwner->GetGuid();

			renderModule.SetConstantBuffer(0, sizeof(ConstBufferPerObject), &perObject, 0);

			const Rendering::Mesh* pRenderingMesh = renderable.m_pMesh;
			renderModule.RenderMesh(*pRenderingMesh);
		}
	}

	void RenderPassObjectId::PostRender(const Systems::RenderableScene& scene)
	{
		m_pObjectIdRenderTarget->EndScene();
	}

	Rendering::RenderTarget* RenderPassObjectId::GetRenderTarget()
	{
		return m_pObjectIdRenderTarget;
	}

	const Core::Guid* RenderPassObjectId::GetGuid(uint32_t objectId) const
	{
		std::map<uint32_t, Core::Guid>::const_iterator it = m_objectIdToGuid.find(objectId);
		if (it == m_objectIdToGuid.cend())
			return nullptr;

		return &it->second;
	}
}
