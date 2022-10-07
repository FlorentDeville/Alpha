/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Material/MaterialMgr.h"

#include "Rendering/Material/Material.h"
namespace Rendering
{
	MaterialMgr::MaterialMgr()
		: Core::Singleton<MaterialMgr>()
		, m_materials()
	{}

	MaterialMgr::~MaterialMgr()
	{
		for (Material* pMaterial : m_materials)
		{
			delete pMaterial;
		}
		m_materials.clear();
	}

	bool MaterialMgr::CreateMaterial(Material** ppMaterial, MaterialId& id)
	{
		*ppMaterial = new Material();
		id = MaterialId(static_cast<uint32_t>(m_materials.size()));
		m_materials.push_back(*ppMaterial);
		return true;
	}

	bool MaterialMgr::DestroyMaterial(MaterialId id)
	{
		Material* pMaterial = m_materials[id.m_id];
		delete pMaterial;
		m_materials[id.m_id] = nullptr;
		return true;
	}

	const Material* MaterialMgr::GetMaterial(MaterialId id) const
	{
		return m_materials[id.m_id];
	}
}
