/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Widgets/Viewport.h"

#include "Rendering/Camera.h"
#include "Rendering/Mesh/Mesh.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Texture/Texture.h"
#include "Rendering/Texture/TextureId.h"
#include "Rendering/Texture/TextureMgr.h"

#include "Widgets/WidgetMgr.h"

#include <DirectXMath.h>
using namespace DirectX;

namespace Widgets
{
	Viewport::Viewport(int width, int height)
		: Widget()
		, m_width(width)
		, m_height(height)
		, m_updateEnabled(false)
	{
		m_pRenderTarget = new Rendering::RenderTarget(width, height, Rendering::BufferFormat::R16G16B16A16_FLOAT);
		m_aspectRatio = m_width / static_cast<float>(m_height);

		OnFocusGained([this](const Widgets::FocusEvent&) { m_updateEnabled = true; });
		OnFocusLost([this](const Widgets::FocusEvent&) { m_updateEnabled = false; });
		OnMouseEnter([this](const Widgets::MouseEvent& ev) -> bool { SetFocus(); return true; });

		SetSizeStyle(Widgets::STRETCH);
	}

	Viewport::~Viewport()
	{
		delete m_pRenderTarget;
	}

	void Viewport::ClearDepthBuffer()
	{
		m_pRenderTarget->ClearDepthBuffer();
	}

	void Viewport::Update(uint64_t dt)
	{
		if(m_updateEnabled)
			m_onUpdate(dt);
	}

	void Viewport::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();
		WidgetMgr& widgetMgr = WidgetMgr::Get();

		const Rendering::PipelineState* pPipelineState = Rendering::PipelineStateMgr::Get().GetPipelineState(WidgetMgr::Get().m_widgetViewportPsoId);
		const Rendering::RootSignature* pRootSignature = Rendering::RootSignatureMgr::Get().GetRootSignature(pPipelineState->GetRootSignatureId());
		const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
		
		Rendering::TextureId renderTargetTextureId = renderModule.GetRenderTargetTextureId(m_pRenderTarget);

		Rendering::Texture* pTexture = Rendering::TextureMgr::Get().GetTexture(renderTargetTextureId);
		ID3D12DescriptorHeap* pSrv = pTexture->GetSRV();

		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

		renderer.BindMaterial(*pPipelineState, *pRootSignature);
		renderer.RenderMesh(*pMesh);
		
		//Set Constant buffer
		DirectX::XMMATRIX wvp;
		ComputeWVPMatrix(windowSize, wvp);

		renderer.SetConstantBuffer(0, sizeof(wvp), &wvp, 0);

		renderer.SetConstantBuffer(1, sizeof(m_size), &m_size, 0);

		Core::Float2 textureRect;
		textureRect.x = (float)pTexture->GetWidth();
		textureRect.y = (float)pTexture->GetHeight();
		renderer.SetConstantBuffer(2, sizeof(textureRect), &textureRect, 0);

		ID3D12GraphicsCommandList2* pCommandList = renderer.GetRenderCommandList();

		//Set texture srv
		ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };
		pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
		pCommandList->SetGraphicsRootDescriptorTable(3, pSrv->GetGPUDescriptorHandleForHeapStart());

		//Draw!!!
		pCommandList->DrawIndexedInstanced(pMesh->GetIndicesCount(), 1, 0, 0, 0);

		Widget::Draw(windowSize, scissor);
	}

	Core::Vec4f Viewport::Compute3dPosition(const Core::UInt2& windowAbsPos, const Core::Mat44f& view, const Core::Mat44f& proj) const
	{
		//get position in viewport widget
		Core::UInt2 absPos(GetScreenX(), GetScreenY());
		Core::UInt2 size = GetSize();

		Core::UInt2 mouseViewportWidgetPos(windowAbsPos.x - absPos.x, windowAbsPos.y - absPos.y);

		Core::UInt2 mouseViewportPos(0, 0);

		//real size of the texture painted on the viewport, copied from the shader code
		float h = 0;
		float w = 0;
		float textureRatio = (float)m_width / (float)m_height;
		{
			h = static_cast<float>(size.y);
			w = h * textureRatio;
			if (w < size.x)
			{
				w = static_cast<float>(size.x);
				h = w / textureRatio;
			}
		}

		if (size.x < w)
		{
			mouseViewportPos.x = static_cast<uint32_t>(mouseViewportWidgetPos.x + (w - size.x) * 0.5f);
		}
		else
		{
			mouseViewportPos.x = mouseViewportWidgetPos.x;
		}

		if (size.y < h)
		{
			mouseViewportPos.y = static_cast<uint32_t>(mouseViewportWidgetPos.y + (h - size.y) * 0.5f);
		}
		else
		{
			mouseViewportPos.y = mouseViewportWidgetPos.y;
		}

		//get mouse 3d position

		//calculate the inverse of the view
		Core::Mat44f invR = view;
		invR.SetRow(3, Core::Vec4f(0, 0, 0, 1));
		invR.Transpose();
		Core::Vec4f viewPos = view.GetT();
		Core::Vec4f invViewPos = (viewPos * invR) * -1;
		invViewPos.Set(3, 1);
		Core::Mat44f invView = invR;
		invView.SetRow(3, invViewPos);

		float mouseScreenSpaceX = ((mouseViewportPos.x / (float)w) * 2 - 1) / proj.GetX().GetX();
		float mouseScreenSpaceY = ((mouseViewportPos.y / (float)h) * -2 + 1) / proj.GetY().GetY();
		Core::Vec4f mousePosition(mouseScreenSpaceX, mouseScreenSpaceY, 1, 1);
		Core::Vec4f mousePosition3d = mousePosition * invView;
		return mousePosition3d;
	}

	void Viewport::Internal_Render()
	{
		m_onPreRender();

		m_pRenderTarget->BeginScene();
		m_onRender();
		m_pRenderTarget->EndScene();
	}
}
