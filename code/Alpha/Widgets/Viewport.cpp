/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Viewport.h"

#include "Rendering/Mesh/Mesh.h"
#include "Rendering/Renderable/RenderableMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Widgets/Message.h"
#include "Widgets/WidgetMgr.h"

#include <DirectXMath.h>
using namespace DirectX;

extern RenderModule* g_pRenderModule;
extern TextureId g_textureId;

extern DirectX::XMVECTOR g_eyePosition;
extern DirectX::XMVECTOR g_euler;
extern DirectX::XMVECTOR g_direction;

namespace Widgets
{
	Viewport::Viewport()
		:Widget()
	{}

	Viewport::~Viewport()
	{}

	void Viewport::Draw()
	{
		const Renderable* pRenderable = g_pRenderableMgr->GetRenderable(WidgetMgr::Get().m_widgetRenderableId);
		const PipelineState* pPipelineState = g_pPipelineStateMgr->GetResource(WidgetMgr::Get().m_widgetViewportPsoId);
		RootSignature* pRootSignature = g_pRootSignatureMgr->GetRootSignature(pPipelineState->GetRootSignatureId());
		const Mesh* pMesh = g_pMeshMgr->GetMesh(pRenderable->GetMeshId());
		Texture* pTexture = g_pTextureMgr->GetResource(g_pRenderModule->GetRenderTextureId());
		ID3D12DescriptorHeap* pSrv = pTexture->GetSRV();

		ID3D12GraphicsCommandList2* pCommandList = g_pRenderModule->GetRenderCommandList();

		pCommandList->SetPipelineState(pPipelineState->GetPipelineState());
		pCommandList->SetGraphicsRootSignature(pRootSignature->GetRootSignature());

		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pCommandList->IASetVertexBuffers(0, 1, &pMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&pMesh->GetIndexBufferView());

		//Set Constant buffer
		DirectX::XMMATRIX wvp;
		ComputeWVPMatrix(wvp);
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

	bool Viewport::Handle(const Message& msg)
	{
		switch(msg.m_id)
		{
		case M_MouseLDown:
			WidgetMgr::Get().SetFocus(this);
			m_previousMousePosition.x = msg.m_low.m_pos[0];
			m_previousMousePosition.y = msg.m_low.m_pos[1];
			return true;
			break;

		case M_KeyDown:
		{
			const float speed = 0.5f;

			DirectX::XMMATRIX orientation = DirectX::XMMatrixRotationRollPitchYawFromVector(g_euler);
			DirectX::XMVECTOR direction = DirectX::XMVector3Transform(g_direction, orientation);
			DirectX::XMVECTOR xAxis = speed * DirectX::XMVector3Transform(DirectX::XMVectorSet(1, 0, 0, 0), orientation);
			DirectX::XMVECTOR zAxis = speed * DirectX::XMVector3Transform(DirectX::XMVectorSet(0, 0, 1, 0), orientation);
			
			char c = (char)msg.m_high;
			switch (c)
			{
			case 'a':
			case 'A':
			{
				g_eyePosition = DirectX::XMVectorAdd(g_eyePosition, -xAxis);
			}
				break;

			case 'd':
			case 'D':
			{
				g_eyePosition = DirectX::XMVectorAdd(g_eyePosition, xAxis);
			}
				break;

			case 'w':
			case 'W':
			{
				g_eyePosition = DirectX::XMVectorAdd(g_eyePosition, zAxis);
			}
				break;

			case 's':
			case 'S':
			{
				g_eyePosition = DirectX::XMVectorAdd(g_eyePosition, -zAxis);
			}
				break;
			}

			return true;
		}
		break;

		case M_MouseMove:
		{
			if (msg.m_high == M_LButton && WidgetMgr::Get().GetFocusedWidget() == this)
			{
				float speed = 0.01f;
				DirectX::XMINT2 dt;
				dt.x = msg.m_low.m_pos[0] - m_previousMousePosition.x;
				dt.y = msg.m_low.m_pos[1] - m_previousMousePosition.y;
				
				//For some reason the order is Roll/Pitch/Yaw instead of Pitch/Roll/Yaw
				DirectX::XMVECTOR offset = DirectX::XMVectorSet(dt.y * speed, dt.x * speed , 0, 0);
				g_euler = DirectX::XMVectorAdd(g_euler, offset);

				m_previousMousePosition.x = msg.m_low.m_pos[0];
				m_previousMousePosition.y = msg.m_low.m_pos[1];
				return true;
			}
		}
		break;

		default:
			return false;
			break;
		}

		return false;
	}
}
