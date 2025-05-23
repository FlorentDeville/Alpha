/********************************************************************/
/* � 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
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

		Container* CreateContainer(const char* seed);
		Container* GetContainer(ContainerId cid);

		// Delete the container from memory and disk.
		bool DeleteContainer(ContainerId cid);

		Container* LoadContainer(ContainerId cid);
		bool SaveContainer(ContainerId cid);

	private:
		std::string m_root;
		std::map<ContainerId, Container*> m_containerMap;

		std::string MakeDirectory(ContainerId cid);
		std::string MakeFilename(ContainerId cid);
	};
}
