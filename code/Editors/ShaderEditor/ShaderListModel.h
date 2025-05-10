/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Models/AbstractViewModel.h"

#include "Systems/Assets/NewAssetId.h"

namespace Editors
{
	class ShaderListModel : public Widgets::AbstractViewModel
	{
	public:
		ShaderListModel();
		~ShaderListModel();

		Widgets::ModelIndex GetParent(const Widgets::ModelIndex&) const;
		Widgets::ModelIndex GetIndex(int row, int column, const Widgets::ModelIndex& parent) const;
		int GetRowCount(const Widgets::ModelIndex& index);
		int GetColumnCount(const Widgets::ModelIndex&) const;
		std::string GetData(const Widgets::ModelIndex&);

		Systems::NewAssetId GetAssetId(const Widgets::ModelIndex& index) const;

	private:
		struct CachedShaderData
		{
			Systems::NewAssetId m_id;
			std::string m_virtualName;
			bool m_modified;
		};

		std::vector<CachedShaderData> m_cache;
	};
}
