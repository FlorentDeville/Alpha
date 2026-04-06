/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Systems/Container/LoadingDomain.h"

#include <map>
#include <string>

namespace Systems
{
	class Container;
	class ContainerId;

	class ContainerMgr : public Core::Singleton<ContainerMgr>
	{
	public:
		ContainerMgr();
		~ContainerMgr();

		void Init(const std::string& root);
		void Shutdown() override;

		//Return a pointer to a container. The container needs to have been loaded or created before or the function returns nullptr.
		//It doesn't increas the ref count.
		Container* GetContainer(ContainerId cid, LoadingDomain domain);

		//Load a container. If the container is already loaded, returns a pointer to the already loaded container and increase the ref count.
		Container* LoadContainer(ContainerId cid, LoadingDomain domain);

		//Decrease the ref count for the container. When reaching 0, it doesn't free the memory right away. The memory is freed when calling GarbageCollect.
		void UnloadContainer(ContainerId cid, LoadingDomain domain);

		//Free the memory for all containers with a refcount of 0
		void GarbageCollect();

		void IncRefCount(ContainerId cid, LoadingDomain domain);
		void DecRefCount(ContainerId cid, LoadingDomain domain);

		// Editor functions

		//Create a new container using the provided container id. If a container already exists on disk using the 
		//provided container id, the function will fail and return nullptr.
		Container* CreateContainer(ContainerId cid);

		bool SaveContainer(ContainerId cid);

		// Delete the container from memory and disk.
		bool DeleteContainer(ContainerId cid);

		bool DoesContainerExistsOnDisk(ContainerId cid) const;

	private:
		struct ContainerRefCount
		{
			Container* m_pPtr;
			int8_t m_count;
		};

		std::string m_root;
		std::map<ContainerId, ContainerRefCount> m_containerMap[static_cast<uint8_t>(LoadingDomain::COUNT)];

		std::string MakeDirectory(ContainerId cid) const;
		std::string MakeFilename(ContainerId cid) const;

		ContainerRefCount* GetContainerRefCount(ContainerId id, LoadingDomain domain);
	};
}
