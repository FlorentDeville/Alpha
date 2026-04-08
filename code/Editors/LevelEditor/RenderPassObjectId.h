/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Guid/Guid.h"

#include "Systems/Rendering/RenderPass/IRenderPass.h"

#include <cstdint>
#include <map>

namespace Rendering
{
	class PipelineState;
	class RenderTarget;
	class RootSignature;
}

namespace Systems
{
	class RenderableObject;
}

namespace Editors
{
	class RenderPassObjectId : public Systems::IRenderPass
	{
	public:
		RenderPassObjectId(int32_t width, int32_t height);
		~RenderPassObjectId();

		void PreRender(const Systems::RenderableScene& scene);
		void Render(const Systems::RenderableScene& scene);
		void PostRender(const Systems::RenderableScene& scene);

		Rendering::RenderTarget* GetRenderTarget();

		const Core::Guid* GetGuid(uint32_t objectId) const;

	private:
		Rendering::RootSignature* m_pObjectIdRootSig;
		Rendering::PipelineState* m_pObjectIdPso;
		Rendering::RenderTarget* m_pObjectIdRenderTarget;

		std::map<uint32_t, Core::Guid> m_objectIdToGuid;

		void RenderObject(const Systems::RenderableObject& obj, uint32_t& objectIdCounter);
	};
}
