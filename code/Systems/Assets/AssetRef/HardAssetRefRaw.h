/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/NewAssetId.h"

namespace Systems
{
	// Non templatized version of HardAssetRef<T>. It is binary identical with HardAssetRef<T> and can be swapped with it.
	// It is used internally by HardAssetRef<T> and by the reflection systems.
	class HardAssetRefRaw
	{
	public:
		HardAssetRefRaw();
		HardAssetRefRaw(NewAssetId id);
		~HardAssetRefRaw();

		bool Resolve();

		bool IsResolved() const;

	private:
		NewAssetId m_id;
		void* m_pPtr;

		template<class T> friend class HardAssetRef;
	};
}
