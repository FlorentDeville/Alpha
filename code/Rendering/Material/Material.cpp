/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Material/Material.h"


#include "Rendering/RootSignature/RootSignatureMgr.h"

namespace Rendering
{
	Material::Material()
		: m_pRootSignature(nullptr)
		, m_pPipelineState(nullptr)
		, m_textureId(-1)
	{}

	Material::~Material()
	{}

	bool Material::Init(RootSignatureId rid, PipelineStateId pid)
	{
		m_pRootSignature = g_pRootSignatureMgr->GetRootSignature(rid);
		m_pPipelineState = g_pPipelineStateMgr->GetResource(pid);

		return true;
	}

	void Material::SetTexture(TextureId id)
	{
		m_textureId = id;
	}
}
