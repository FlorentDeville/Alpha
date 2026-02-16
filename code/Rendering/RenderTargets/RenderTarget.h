/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Rendering/Texture/TextureId.h"

#include <d3d12.h>

namespace Core
{
	class Vec4f;
}

namespace Rendering
{
	class DescriptorHeap;
	class RenderModule;
	class Texture;

	class RenderTarget
	{
		friend RenderModule;

	public:
		RenderTarget(int width, int height);
		RenderTarget(int width, int height, DXGI_FORMAT format);
		RenderTarget(int width, int height, DXGI_FORMAT format, const Core::Vec4f& clearColor);
		~RenderTarget();

		void BeginScene();
		void EndScene();

		void ClearDepthBuffer();

		void CopyToReabackBuffer(Texture* pTexture);

		Rendering::Texture* GetColorTexture();

	private:
		DescriptorHeap* m_pRTVHeap;				// memory heap for the render target view
		DescriptorHeap* m_pDSVHeap;				// memory heap for the depth stencil view

		Rendering::TextureId m_textureId;		// Id of the color texture.
		Rendering::Texture* m_pTexture;			// Texture used to store color values
		D3D12_CPU_DESCRIPTOR_HANDLE m_rtv;		// render target view
		ID3D12Resource* m_pDepthBuffer;			// Depth/stencil buffer for this render target
		D3D12_CPU_DESCRIPTOR_HANDLE m_dsv;		// Depth stencil view
		D3D12_VIEWPORT m_viewport;				// The sub rectangle of the render target where to render the scene
		D3D12_RECT m_scissorRect;				// A rectangle within the viewport cullling out the pixels outside the scissor.

		float m_clearColor[4];

		void CreateDepthBuffer(int width, int height);
	};
}
