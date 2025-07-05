/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Icon.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/Texture/Texture.h"
#include "Rendering/Texture/TextureMgr.h"

#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Icon::Icon()
		: Widget()
		, m_textureId(Rendering::TextureId::INVALID)
	{}

	Icon::Icon(Rendering::TextureId textureId)
	{
		m_textureId = textureId;
		const Rendering::Texture* pTexture = Rendering::TextureMgr::Get().GetTexture(textureId);

		uint32_t w = static_cast<uint32_t>(pTexture->GetWidth());
		uint32_t h = static_cast<uint32_t>(pTexture->GetHeight());
		SetSize(DirectX::XMUINT2(w, h));
		SetX(0);
		SetY(0);
	}

	Icon::Icon(const std::string& path)
		: Widget()
	{
		Rendering::Texture* pTexture = nullptr;
		Rendering::TextureMgr::Get().CreateTexture(&pTexture, m_textureId);

		pTexture->Init(path);

		uint32_t w = static_cast<uint32_t>(pTexture->GetWidth());
		uint32_t h = static_cast<uint32_t>(pTexture->GetHeight());
		SetSize(DirectX::XMUINT2(w, h));
		SetX(0);
		SetY(0);
	}

	Icon::Icon(const DirectX::XMINT2& pos, const DirectX::XMUINT2 size, const std::string& path)
		: Widget(size.x, size.y, pos.x, pos.y)
	{
		Rendering::Texture* pTexture = nullptr;
		Rendering::TextureMgr::Get().CreateTexture(&pTexture, m_textureId);

		pTexture->Init(path);
	}

	Icon::~Icon()
	{}

	void Icon::Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor)
	{
		DirectX::XMMATRIX wvp;
		ComputeWVPMatrix(windowSize, wvp);

		WidgetMgr& widgetMgr = WidgetMgr::Get();
		Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();
		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

		const Rendering::Material* pMaterial = materialMgr.GetMaterial(widgetMgr.m_iconMaterialId);
		renderer.BindMaterial(*pMaterial, wvp);

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
}
