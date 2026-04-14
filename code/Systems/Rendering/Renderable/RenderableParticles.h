/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Rendering
{
	class PipelineState;
	class RootSignature;
	class Texture;
}
namespace Systems
{
	//I would prefer to keep it generic but I would need to either have dynamic material, which I don't,
	//or make some kind of specific MaterialAsset for particle
	class RenderableParticles
	{
	public:
		Rendering::Texture* m_pBufferPositions;
		Rendering::PipelineState* m_pPso;
		Rendering::RootSignature* m_pRootsig;
		uint32_t m_instanceCount;
	};
}
