/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

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
		m_pRenderTarget = Rendering::RenderModule::Get().CreateRenderTarget(width, height);
		m_aspectRatio = m_width / static_cast<float>(m_height);

		OnFocusGained([this](const Widgets::FocusEvent&) { m_updateEnabled = true; });
		OnFocusLost([this](const Widgets::FocusEvent&) { m_updateEnabled = false; });
		OnMouseEnter([this](const Widgets::MouseEvent& ev) -> bool { SetFocus(); return true; });

		SetSizeStyle(Widgets::Widget::STRETCH);
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

		ID3D12GraphicsCommandList2* pCommandList = Rendering::RenderModule::Get().GetRenderCommandList();

		pCommandList->SetPipelineState(pPipelineState->GetPipelineState());
		pCommandList->SetGraphicsRootSignature(pRootSignature->GetRootSignature());

		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pCommandList->IASetVertexBuffers(0, 1, &pMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&pMesh->GetIndexBufferView());

		//Set Constant buffer
		DirectX::XMMATRIX wvp;
		ComputeWVPMatrix(windowSize, wvp);
		pCommandList->SetGraphicsRoot32BitConstants(0, sizeof(wvp) / 4, &wvp, 0);

		pCommandList->SetGraphicsRoot32BitConstants(1, sizeof(m_size) / 4, &m_size, 0);

		DirectX::XMFLOAT2 textureRect;
		textureRect.x = (float)pTexture->GetWidth();
		textureRect.y = (float)pTexture->GetHeight();
		pCommandList->SetGraphicsRoot32BitConstants(2, sizeof(textureRect) / 4, &textureRect, 0);

		//Set texture srv
		ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };
		pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
		pCommandList->SetGraphicsRootDescriptorTable(3, pSrv->GetGPUDescriptorHandleForHeapStart());

		//Draw!!!
		pCommandList->DrawIndexedInstanced(pMesh->GetIndicesCount(), 1, 0, 0, 0);
	}

	DirectX::XMVECTOR Viewport::Compute3dPosition(const DirectX::XMUINT2& windowAbsPos) const
	{
		const Rendering::Camera* pCamera = Rendering::RenderModule::Get().GetConstCamera();
		const XMMATRIX& view = pCamera->GetViewMatrix();
		const XMMATRIX& proj = pCamera->GetProjectionMatrix();

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

		DirectX::XMFLOAT2 mouseScreenSpace;
		mouseScreenSpace.x = ((mouseViewportPos.x / (float)w) * 2 - 1) / proj.r[0].m128_f32[0];
		mouseScreenSpace.y = ((mouseViewportPos.y / (float)h) * -2 + 1) / proj.r[1].m128_f32[1];

		//get mouse 3d position
		XMMATRIX invView = XMMatrixInverse(nullptr, view);
		XMVECTOR mousePosition = XMVectorSet(mouseScreenSpace.x, mouseScreenSpace.y, 1, 1);

		XMVECTOR mousePosition3d = XMVector3Transform(mousePosition, invView);

		return mousePosition3d;
	}

	void Viewport::Internal_Render()
	{
		m_pRenderTarget->BeginScene();
		m_onRender();
		m_pRenderTarget->EndScene();
	}
}
