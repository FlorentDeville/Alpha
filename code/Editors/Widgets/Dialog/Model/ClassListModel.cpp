/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/Dialog/Model/ClassListModel.h"

#include "Core/Reflection/ReflectionMgr.h"

#include "Widgets/Models/ModelIndex.h"

#include <algorithm>

namespace Editors
{
	ClassListModel::ClassListModel(const Core::Sid& baseClassSid, bool includeBaseClass)
		: Widgets::AbstractViewModel()
	{
		Core::ReflectionMgr::Get().ForEachType([this, baseClassSid, includeBaseClass](const Core::TypeDescriptor* pType)
			{
				bool addAsBaseClass = (includeBaseClass && pType->GetSid() == baseClassSid);
				bool addAsDeriveClass = pType->InheritsFrom(baseClassSid);

				if (!addAsBaseClass && !addAsDeriveClass)
					return;

				CachedItem& cache = m_cache.PushBackDefault();
				cache.m_pType = pType;
				cache.m_name = CreateCleanTypeName(pType);
			});

		std::sort(m_cache.begin(), m_cache.end(), [](const CachedItem& data1, const CachedItem& data2) { return data1.m_name < data2.m_name; });
	}

	ClassListModel::~ClassListModel()
	{ }

	Widgets::ModelIndex ClassListModel::GetParent(const Widgets::ModelIndex& child) const
	{
		return Widgets::ModelIndex();
	}

	Widgets::ModelIndex ClassListModel::GetIndex(int row, int column, const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return Widgets::ModelIndex();

		if (!m_cache.IsValidIndex(row))
			return Widgets::ModelIndex();

		return CreateIndex(row, column, &m_cache[row]);
	}

	int ClassListModel::GetRowCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return m_cache.GetSize();
	}

	int ClassListModel::GetColumnCount(const Widgets::ModelIndex& parent) const
	{
		return 1;
	}

	std::string ClassListModel::GetData(const Widgets::ModelIndex& index)
	{
		if (!index.IsValid())
			return "";

		int row = index.GetRow();
		if (!m_cache.IsValidIndex(row))
			return "";

		return m_cache[row].m_name;
	}

	std::string ClassListModel::GetHeaderData(int columnIndex)
	{
		if (columnIndex == 0)
			return "Class Name";

		return "";
	}

	const Core::TypeDescriptor* ClassListModel::GetType(const Widgets::ModelIndex& index)
	{
		if (!index.IsValid())
			return nullptr;

		return m_cache[index.GetRow()].m_pType;
	}

	std::string ClassListModel::CreateCleanTypeName(const Core::TypeDescriptor* pType) const
	{
		const std::string& typeName = pType->GetName();

		//remove the namespace
		std::string cleanedName;
		size_t pos = typeName.find_last_of(':');
		if (pos != std::string::npos)
			cleanedName = typeName.substr(pos + 1);
		else
			cleanedName = typeName;

		std::string splittedString;

		//split the words
		size_t start = 0;
		for (int ii = 0; ii < cleanedName.size(); ++ii)
		{
			if (!std::isupper(cleanedName[ii]))
				continue;

			size_t end = ii;
			splittedString = splittedString + " " + cleanedName.substr(start, end - start);
			start = end;
			++ii;
		}

		splittedString = splittedString + " " + cleanedName.substr(start, cleanedName.size() - start);

		return splittedString;
	}
}
