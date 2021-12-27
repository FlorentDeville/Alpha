/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Renderable/RenderableMgr.h"

#include "Rendering/Renderable/Renderable.h"


RenderableMgr::RenderableMgr()
	: m_renderables()
	, m_quadId()
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

RenderableId RenderableMgr::CreateQuad(MeshId meshId, PipelineStateId pipelineStateId)
{
	m_quadId = CreateRenderable(meshId, pipelineStateId);
	return m_quadId;
}

Renderable* RenderableMgr::GetRenderable(RenderableId renderableId) const
{
	assert(renderableId.m_id < m_renderables.size());
	return m_renderables[renderableId.m_id];
}

Renderable* RenderableMgr::GetQuad() const
{
	return GetRenderable(m_quadId);
}

RenderableMgr* g_pRenderableMgr = nullptr;
