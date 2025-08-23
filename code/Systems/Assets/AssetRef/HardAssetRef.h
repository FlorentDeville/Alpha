/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/NewAssetId.h"

namespace Systems
{
	// HardAssetRef is a reference to an asset meant to be used in any serialized object.
	// The referenced asset is loaded automatically when the object containing this HardAssetRef is loaded.
	// It is read only and cannot be reset.
	// It is meant to be used like a pointer so you can use operators -> and *.
	template<typename T> class HardAssetRef
	{
	public:
		HardAssetRef();
		HardAssetRef(NewAssetId id);
		~HardAssetRef();

		T* operator->();
		const T* operator->() const;

		T& operator*();
		const T& operator*() const;

		// Get a pointer to the asset if the asset is loaded, otherwise the pointer stays null. 
		// Return true if a valid pointer is found, false otherwise.
		bool Resolve();

	private:
		NewAssetId m_id;
		T* m_pPtr;
	};
}

#include "Systems/Assets/AssetRef/HardAssetRef.inl"
