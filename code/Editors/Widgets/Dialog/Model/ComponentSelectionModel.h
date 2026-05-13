/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Sid/Sid.h"

#include "Widgets/Models/AbstractViewModel.h"

namespace Systems
{
	class GameObject;
}

namespace Editors
{
	class ComponentSelectionModel : public Widgets::AbstractViewModel
	{
	public:
		enum Columns
		{
			Id,
			Name,
			Guid,

			Count
		};

		struct MetadataCache
		{
			std::string m_id;
			std::string m_name;
			std::string m_guid;
		};

		ComponentSelectionModel(const Systems::GameObject* pGo, Core::Sid componentTypeSid);
		~ComponentSelectionModel();

		Widgets::ModelIndex GetParent(const Widgets::ModelIndex& child) const override;
		Widgets::ModelIndex GetIndex(int row, int column, const Widgets::ModelIndex& parent) const override;
		int GetRowCount(const Widgets::ModelIndex& parent) const override;
		int GetColumnCount(const Widgets::ModelIndex& parent) const override;
		std::string GetHeaderData(int columnIndex);
		std::string GetData(const Widgets::ModelIndex& idx) override;

	private:
		Core::Array<MetadataCache> m_cache;
	};
}
