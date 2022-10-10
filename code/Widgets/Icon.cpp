/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Icon.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"

#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Icon::Icon(const DirectX::XMINT2& pos, const DirectX::XMUINT2 size, const std::string& path)
		: Widget(size.x, size.y, pos.x, pos.y)
	{
		Texture* pTexture = RenderModule::Get().GetTextureMgr().CreateResource(m_textureId, path);
		pTexture->Init(path);
	}

	Icon::~Icon()
	{}

	void Icon::Draw(const DirectX::XMFLOAT2& windowSize)
	{
		DirectX::XMMATRIX wvp;
		ComputeWVPMatrix(windowSize, wvp);

		WidgetMgr& widgetMgr = WidgetMgr::Get();
		Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();
		RenderModule& renderer = RenderModule::Get();

		const Rendering::Material* pMaterial = materialMgr.GetMaterial(widgetMgr.m_iconMaterialId);
		renderer.BindMaterial(*pMaterial, wvp);

		ID3D12DescriptorHeap* pSrv = renderer.GetTextureMgr().GetResource(m_textureId)->GetSRV();
		ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };
		renderer.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
		renderer.GetRenderCommandList()->SetGraphicsRootDescriptorTable(1, pSrv->GetGPUDescriptorHandleForHeapStart());

		const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
		renderer.RenderMesh(*pMesh);
	}
}
