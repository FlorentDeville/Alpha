/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Assets/NewAssetId.h"
#include "Systems/Container/ContainerMgr.h"

namespace Systems
{
	// Non templatized version of HardAssetRef<T>. It is binary identical with HardAssetRef<T> and can be swapped with it.
	// It is used internally by HardAssetRef<T> and by the reflection systems.
	class HardAssetRefRaw
	{
	public:
		HardAssetRefRaw();
		explicit HardAssetRefRaw(NewAssetId id);
		HardAssetRefRaw(const HardAssetRefRaw& copy) = delete;
		~HardAssetRefRaw();

		HardAssetRefRaw& operator=(const HardAssetRefRaw& other);

		bool IsResolved() const;

		NewAssetId GetAssetId() const;

		bool Load(LoadingDomain domain);

	private:
		NewAssetId m_id;
		void* m_pPtr;
		LoadingDomain m_domain;

		template<class T> friend class HardAssetRef;
	};
}
