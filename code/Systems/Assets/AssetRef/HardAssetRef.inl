/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetObjects/AssetUtil.h"

namespace Systems
{
	template<typename T> HardAssetRef<T>::HardAssetRef()
		: m_id(Systems::NewAssetId::INVALID)
		, m_pPtr(nullptr)
	{ }

	template<typename T> HardAssetRef<T>::HardAssetRef(NewAssetId id)
		: m_id(id)
		, m_pPtr(nullptr)
	{ }
		
	template<typename T> HardAssetRef<T>::~HardAssetRef()
	{ }

	template<typename T> T* HardAssetRef<T>::operator->()
	{
		return m_pPtr;
	}

	template<typename T> const T* HardAssetRef<T>::operator->() const
	{
		return m_pPtr;
	}

	template<typename T> T& HardAssetRef<T>::operator*()
	{
		return *m_pPtr;
	}

	template<typename T> const T& HardAssetRef<T>::operator*() const
	{
		return *m_pPtr;
	}

	template<typename T> bool HardAssetRef<T>::Resolve()
	{
		m_pPtr = AssetUtil::GetAsset(m_id);
		return m_pPtr != nullptr;
	}

	template<typename T> T* HardAssetRef<T>::GetPtr()
	{
		return m_pPtr;
	}
}
