/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Systems/Assets/NewAssetId.h"

#include <string>
#include <vector>

namespace Editors
{
	class ShaderEditorModule : public Core::Singleton<ShaderEditorModule>
	{
	public:
		void Init() override;

		bool NewShader(const std::string& virtualName);
		bool SaveShader(Systems::NewAssetId id);

		const std::vector<Systems::NewAssetId>& GetAllShaders() const;

	private:
		std::vector<Systems::NewAssetId> m_allShaders;
	};
}
