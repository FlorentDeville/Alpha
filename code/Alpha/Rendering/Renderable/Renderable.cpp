/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Renderable/Renderable.h"

#include "Rendering/PipelineState/PipelineState.h"

#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"

#include "Rendering/Mesh/Mesh.h"
#include "Rendering/Mesh/MeshMgr.h"

Renderable::Renderable(MeshId meshId, PipelineStateId pipelineStateId)
	: m_meshId(meshId)
	, m_pipelineStateId(pipelineStateId)
{}

Renderable::~Renderable()
{}

MeshId Renderable::GetMeshId() const
{
	return m_meshId;
}

PipelineStateId Renderable::GetPipeplineStateId() const
{
	return m_pipelineStateId;
}
