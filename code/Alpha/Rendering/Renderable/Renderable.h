/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/PipelineState/PipelineStateId.h"

class Renderable
{
public:
	Renderable(MeshId meshId, PipelineStateId pipelineStateId);
	~Renderable();

	MeshId GetMeshId() const;
	PipelineStateId GetPipeplineStateId() const;

private:
	MeshId m_meshId;
	PipelineStateId m_pipelineStateId;
};
