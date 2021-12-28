/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Rendering/Mesh/MeshId.h"
#include "Rendering/PipelineState/PipelineStateId.h"
#include "Rendering/Renderable/RenderableId.h"

#include <vector>

class Renderable;

class RenderableMgr
{
public:
	RenderableMgr();
	~RenderableMgr();

	RenderableId CreateRenderable(MeshId meshId, PipelineStateId pipelineStateId);

	Renderable* GetRenderable(RenderableId renderableId) const;

private:
	std::vector<Renderable*> m_renderables;
};

extern RenderableMgr* g_pRenderableMgr;
