/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <vector>

#include "Resource/Resource.h"

//Resource id name
#define RESOURCE_ID(ResType) ResType##Id

//Declare resource id type
#define DECLARE_RESOURCE_ID(ResType) using RESOURCE_ID(ResType) = size_t

//Resource mgr typename
#define RESOURCE_MGR(ResType) ResourceMgr<ResType, RESOURCE_ID(ResType)>

//Resource mgr pointer
#define RESOURCE_MGR_PTR(ResType) ResourceMgr<ResType, RESOURCE_ID(ResType)>*

//#define DECLARE_RESOURCE_MGR(ResType) ResourceMgr<ResType, ResType##Id> g_##ResType##Mgr;

template<class T, typename Id> class ResourceMgr
{
public:
	ResourceMgr();
	~ResourceMgr();

	void Init();
	void Release();

	T* CreateResource(Id& id, const std::string& name);
	
	T* GetResource(Id id);
	const T* GetResource(Id id) const;

	void ReleaseResource(Id id);

public:
	std::vector<T*> m_resources;
};

template <class T, typename Id> ResourceMgr<T, Id>::ResourceMgr()
	: m_resources()
{}

template <class T, typename Id> ResourceMgr<T, Id>::~ResourceMgr()
{}

template <class T, typename Id> void ResourceMgr<T, Id>::Init()
{}

template <class T, typename Id> void ResourceMgr<T, Id>::Release()
{
	for (T* ptr : m_resources)
	{
		if (ptr)
		{
			Resource* pResource = static_cast<Resource*>(ptr);
			pResource->Release();
		}
	}
}

template <class T, typename Id> T* ResourceMgr<T, Id>::CreateResource(Id& id, const std::string& name)
{
	id = m_resources.size();
	T* pResource = new T(name);
	m_resources.push_back(pResource);
	return pResource;
}

template <class T, typename Id> T* ResourceMgr<T, Id>::GetResource(Id id)
{
	return m_resources(id);
}

template <class T, typename Id> const T* ResourceMgr<T, Id>::GetResource(Id id) const
{
	return m_resources(id);
}

template <class T, typename Id> void ResourceMgr<T, Id>::ReleaseResource(Id id)
{
	T* ptr = m_resources[id];
	if (ptr)
	{
		Resource* pResource = static_cast<Resource*>(ptr);
		pResource->Release();
		delete pResource;
		m_resources[id] = nullptr;
	}
}
