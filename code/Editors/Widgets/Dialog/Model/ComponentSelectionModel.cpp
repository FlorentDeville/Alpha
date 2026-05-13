/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/Dialog/Model/ComponentSelectionModel.h"

#include "Systems/Objects/GameObject.h"

#include "Widgets/Models/ModelIndex.h"

namespace Editors
{
	ComponentSelectionModel::ComponentSelectionModel(const Systems::GameObject* pGo, Core::Sid componentTypeSid)
	{
		const Core::Array<Systems::GameComponent*>& components = pGo->GetComponents();
		for (uint32_t ii = 0; ii < components.GetSize(); ++ii)
		{
			const Systems::GameComponent* pComponent = components[ii];
			if (!pComponent)
				continue;

			MetadataCache& cache = m_cache.PushBackDefault();
			cache.m_id = std::to_string(ii);
			cache.m_guid.resize(37, '\0');
			pComponent->GetGuid().ToString(cache.m_guid.data(), static_cast<uint32_t>(cache.m_guid.size()));

			cache.m_name = pComponent->GetTypeDescriptor()->GetName();
		}
	}

	ComponentSelectionModel::~ComponentSelectionModel()
	{ }

	Widgets::ModelIndex ComponentSelectionModel::GetParent(const Widgets::ModelIndex& child) const
	{
		return Widgets::ModelIndex();
	}

	Widgets::ModelIndex ComponentSelectionModel::GetIndex(int row, int column, const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return Widgets::ModelIndex();

		if (!m_cache.IsValidIndex(row))
			return Widgets::ModelIndex();

		if (column < 0 || column >= Columns::Count)
			return Widgets::ModelIndex();

		return CreateIndex(row, column, &(m_cache[row]));
	}

	int ComponentSelectionModel::GetRowCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return m_cache.GetSize();
	}

	int ComponentSelectionModel::GetColumnCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return Columns::Count;
	}

	std::string ComponentSelectionModel::GetHeaderData(int columnIndex)
	{
		switch (columnIndex)
		{
		case Columns::Id:
			return "Index";
			break;

		case Columns::Name:
			return "Name";
			break;

		case Columns::Guid:
			return "Guid";
			break;
		}

		return "__ERROR__";
	}

	std::string ComponentSelectionModel::GetData(const Widgets::ModelIndex& idx)
	{
		const MetadataCache* pMetadata = idx.GetConstDataPointer<MetadataCache>();
		if (!pMetadata)
			return "__ERROR__";

		switch (idx.GetColumn())
		{
		case Columns::Id:
			return pMetadata->m_id;
			break;

		case Columns::Name:
			return pMetadata->m_name;
			break;

		case Columns::Guid:
			return pMetadata->m_guid;
			break;
		}

		return "__ERROR__";
	}
}
