/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Icon.h"

#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/Texture/Texture.h"
#include "Rendering/Texture/TextureMgr.h"

#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Icon::Icon()
		: Widget()
		, m_textureId(Rendering::TextureId::INVALID)
		, m_isHidden(false)
	{}

	Icon::Icon(Rendering::TextureId textureId)
	{
		m_isHidden = false;
		m_textureId = textureId;
		const Rendering::Texture* pTexture = Rendering::TextureMgr::Get().GetTexture(textureId);

		uint32_t w = static_cast<uint32_t>(pTexture->GetWidth());
		uint32_t h = static_cast<uint32_t>(pTexture->GetHeight());
		SetSize(Core::UInt2(w, h));
		SetX(0);
		SetY(0);
	}

	Icon::Icon(const std::string& path)
		: Widget()
	{
		m_isHidden = false;
		Rendering::Texture* pTexture = nullptr;
		Rendering::TextureMgr::Get().CreateTexture(&pTexture, m_textureId);

		pTexture->Init(path);

		uint32_t w = static_cast<uint32_t>(pTexture->GetWidth());
		uint32_t h = static_cast<uint32_t>(pTexture->GetHeight());
		SetSize(Core::UInt2(w, h));
		SetX(0);
		SetY(0);
	}

	Icon::Icon(const DirectX::XMINT2& pos, const DirectX::XMUINT2 size, const std::string& path)
		: Widget(size.x, size.y, pos.x, pos.y)
	{
		m_isHidden = false;
		Rendering::Texture* pTexture = nullptr;
		Rendering::TextureMgr::Get().CreateTexture(&pTexture, m_textureId);

		pTexture->Init(path);
	}

	Icon::~Icon()
	{}

	void Icon::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		if (m_isHidden)
			return;

		DirectX::XMMATRIX wvp;
		ComputeWVPMatrix(windowSize, wvp);

		WidgetMgr& widgetMgr = WidgetMgr::Get();
		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();
		Rendering::PipelineStateMgr& psoMgr = Rendering::PipelineStateMgr::Get();

		const Rendering::PipelineState* pPso = psoMgr.GetPipelineState(widgetMgr.GetIconWidgetPsoId());
		renderer.BindMaterial(*pPso);

		renderer.SetConstantBuffer(0, sizeof(wvp), &wvp, 0);

		ID3D12DescriptorHeap* pSrv = Rendering::TextureMgr::Get().GetTexture(m_textureId)->GetSRV();
		ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };
		renderer.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
		renderer.GetRenderCommandList()->SetGraphicsRootDescriptorTable(1, pSrv->GetGPUDescriptorHandleForHeapStart());

		const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
		renderer.RenderMesh(*pMesh);
	}

	void Icon::SetTextureId(Rendering::TextureId tid)
	{
		m_textureId = tid;
	}

	Rendering::TextureId Icon::GetTextureId() const
	{
		return m_textureId;
	}

	void Icon::Hide()
	{
		m_isHidden = true;
	}

	void Icon::Show()
	{
		m_isHidden = false;
	}
}
