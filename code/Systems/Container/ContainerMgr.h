/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

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

		//Create a new container using the provided container id. If a container already exists on disk using the 
		//provided container id, the function will fail and return nullptr.
		Container* CreateContainer(ContainerId cid);

		//Return a pointer to a container. The container needs to have been loaded or created before.
		Container* GetContainer(ContainerId cid);

		// Delete the container from memory and disk.
		bool DeleteContainer(ContainerId cid);

		Container* LoadContainer(ContainerId cid);
		bool SaveContainer(ContainerId cid);

		bool DoesContainerExistsOnDisk(ContainerId cid) const;

	private:
		std::string m_root;
		std::map<ContainerId, Container*> m_containerMap;

		std::string MakeDirectory(ContainerId cid) const;
		std::string MakeFilename(ContainerId cid) const;
	};
}
