/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Rendering/RenderPass/RenderPassBloom.h"

#include "Core/Math/Vec4f.h"

#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/Texture/Texture.h"

namespace Systems
{
	RenderPassBloom::RenderPassBloom(uint32_t width, uint32_t height, uint32_t mipCount)
		: IRenderPass()
		, m_mipCount(mipCount)
		, m_pFrameBuffer(nullptr)
	{
		m_ppRenderTarget = new Rendering::RenderTarget* [mipCount];

		uint32_t mipWidth = width;
		uint32_t mipHeight = height;

		for (uint32_t ii = 0; ii < mipCount; ++ii)
		{
			mipWidth = mipWidth / 2;
			mipHeight = mipHeight / 2;

			m_ppRenderTarget[ii] = new Rendering::RenderTarget(mipWidth, mipHeight, Rendering::ResourceFormat::R16G16B16A16_FLOAT, Core::Vec4f());
		}
	}

	RenderPassBloom::~RenderPassBloom()
	{
		for (uint32_t ii = 0; ii < m_mipCount; ++ii)
			delete m_ppRenderTarget[ii];

		delete m_ppRenderTarget;
	}

	void RenderPassBloom::SetFrameBuffer(Rendering::Texture* pFrameBuffer)
	{
		m_pFrameBuffer = pFrameBuffer;
	}

	void RenderPassBloom::PreRender(const RenderableScene& scene)
	{
		m_pFrameBuffer->TransitionToShaderResource();
	}

	void RenderPassBloom::Render(const RenderableScene& scene)
	{
		//I don't need to extract the emissive?

		//first downsample

		//set the pso, rootsig for downsampling

		for (uint32_t ii = 0; ii < m_mipCount; ++ii)
		{
			// bind the rtv
			// bind the texture
			// render null triangle	
		}

		//now upsample

		//set the pso and rootsig for upsampling
		for (uint32_t ii = m_mipCount - 1; ii >= 0; --ii)
		{
			//bind the rtv
			//bind the texture
			//render null triangle
		}

		//composition
		//add frame buffer and mip0
	}

	void RenderPassBloom::PostRender(const RenderableScene& scene)
	{ }
}
