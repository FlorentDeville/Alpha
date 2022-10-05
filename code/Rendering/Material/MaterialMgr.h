/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "Rendering/Material/MaterialId.h"

#include <vector>

namespace Rendering
{
	class Material;

	class MaterialMgr : public Core::Singleton<MaterialMgr>
	{
	public:
		MaterialMgr();
		~MaterialMgr();

		bool CreateMaterial(Material** ppMaterial, MaterialId& id);
		bool DestroyMaterial(MaterialId id);

		const Material* GetMaterial(MaterialId) const;

	private:
		std::vector<Material*> m_materials;
	};
}