/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Renderable/RenderableMgr.h"

#include "Rendering/Renderable/Renderable.h"

#include <assert.h>

RenderableMgr::RenderableMgr()
	: m_renderables()
{}

RenderableMgr::~RenderableMgr()
{
	for (Renderable* pRenderable : m_renderables)
		delete pRenderable;
}

RenderableId RenderableMgr::CreateRenderable(MeshId meshId, PipelineStateId pipelineStateId)
{
	RenderableId id;
	id.m_id = m_renderables.size();
	m_renderables.push_back(new Renderable(meshId, pipelineStateId));
	return id;
}

Renderable* RenderableMgr::GetRenderable(RenderableId renderableId) const
{
	assert(renderableId.m_id < m_renderables.size());
	return m_renderables[renderableId.m_id];
}

RenderableMgr* g_pRenderableMgr = nullptr;
