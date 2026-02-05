/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

namespace Systems
{
	template<typename T> HardAssetRef<T>::HardAssetRef()
		: m_internalRef()
	{ }

	template<typename T> HardAssetRef<T>::HardAssetRef(NewAssetId id)
		: m_internalRef(id)
	{ }
		
	template<typename T> HardAssetRef<T>::~HardAssetRef()
	{ }

	template<typename T> T* HardAssetRef<T>::operator->()
	{
		return static_cast<T*>(m_internalRef.m_pPtr);
	}

	template<typename T> const T* HardAssetRef<T>::operator->() const
	{
		return static_cast<T*>(m_internalRef.m_pPtr);
	}

	template<typename T> T& HardAssetRef<T>::operator*()
	{
		return *static_cast<T*>(m_internalRef.m_pPtr);
	}

	template<typename T> const T& HardAssetRef<T>::operator*() const
	{
		return *static_cast<T*>(m_internalRef.m_pPtr);
	}

	template<typename T> bool HardAssetRef<T>::Resolve()
	{
		return m_internalRef.Resolve();
	}

	template<typename T> T* HardAssetRef<T>::GetPtr()
	{
		return static_cast<T*>(m_internalRef.m_pPtr);
	}
}
