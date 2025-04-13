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

		Container* Create(const char* seed);
		Container* Get(ContainerId cid);

		bool Load(ContainerId cid);
		bool Save(ContainerId cid);

	private:
		std::string m_root;
		std::map<ContainerId, Container*> m_containerMap;

		std::string MakeFilename(ContainerId cid);
	};
}
