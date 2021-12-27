/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/PipelineState/PipelineStateId.h"
#include "Rendering/Renderable/RenderableId.h"

class Renderable;

class RenderableMgr
{
public:
	RenderableMgr();
	~RenderableMgr();

	RenderableId CreateRenderable(MeshId meshId, PipelineStateId pipelineStateId);
	RenderableId CreateQuad(MeshId meshId, PipelineStateId pipelineStateId);
	
	Renderable* GetRenderable(RenderableId renderableId) const;
	Renderable* GetQuad() const;

private:
	std::vector<Renderable*> m_renderables;

	RenderableId m_quadId;

};

extern RenderableMgr* g_pRenderableMgr;
