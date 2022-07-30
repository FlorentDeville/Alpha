/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Rendering/Texture/Texture.h"
#include "Rendering/DescriptorHeap.h"

class RenderModule;

namespace Rendering
{
	//class DescriptorHeap;

	class RenderTarget
	{
		friend RenderModule;

	public:
		RenderTarget(int numFrame, int width, int height);
		~RenderTarget();

	private:
		DescriptorHeap* m_pRTVHeap;				// memory heap for the render target view
		DescriptorHeap* m_pDSVHeap;				// memory heap for the depth stencil view

		int m_numFrames;						// number of buffers
		TextureId* m_renderTargetsId;			// Ids of the textures used as render targets
		Texture** m_renderTargets;				// Array of textures used as render target
		D3D12_CPU_DESCRIPTOR_HANDLE* m_rtv;		// Array of render target view
		ID3D12Resource* m_pDepthBuffer;			// Depth buffer for this render target
		D3D12_CPU_DESCRIPTOR_HANDLE m_dsv;		// Depth stencil view
		D3D12_VIEWPORT m_viewport;				// The sub rectangle of the render target where to render the scene
		D3D12_RECT m_scissorRect;				// A rectangle within the viewport cullling out the pixels outside the scissor.

		void CreateDepthBuffer(int width, int height);
	};
}
