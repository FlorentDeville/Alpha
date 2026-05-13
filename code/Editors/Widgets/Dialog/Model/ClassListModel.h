/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Sid/Sid.h"
#include "Widgets/Models/AbstractViewModel.h"

namespace Core
{
	class TypeDescriptor;
}

namespace Editors
{
	class ClassListModel : public Widgets::AbstractViewModel
	{
	public:
		ClassListModel(const Core::Sid& baseClassSid, bool includeBaseClass);
		~ClassListModel();

		//AbstractViewModel implementation
		Widgets::ModelIndex GetParent(const Widgets::ModelIndex& child) const override;
		Widgets::ModelIndex GetIndex(int row, int column, const Widgets::ModelIndex& parent) const override;
		int GetRowCount(const Widgets::ModelIndex& parent) const override;
		int GetColumnCount(const Widgets::ModelIndex& parent) const override;
		std::string GetData(const Widgets::ModelIndex& index) override;
		std::string GetHeaderData(int columnIndex);

		const Core::TypeDescriptor* GetType(const Widgets::ModelIndex& index);

	private:
		struct CachedItem
		{
			std::string m_name;
			const Core::TypeDescriptor* m_pType;
		};

		Core::Array<CachedItem> m_cache;

		std::string CreateCleanTypeName(const Core::TypeDescriptor* pType) const;
	};
}
