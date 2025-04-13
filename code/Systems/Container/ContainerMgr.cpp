/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Container/ContainerMgr.h"

#include "Core/Sid/Sid.h"

#include "Systems/Container/Container.h"
#include "Systems/Container/ContainerId.h"

namespace Systems
{
	ContainerMgr::ContainerMgr()
		: m_containerMap()
		, m_root()
	{ }

	ContainerMgr::~ContainerMgr()
	{ }

	void ContainerMgr::Init(const std::string& root)
	{
		m_root = root;
	}

	void ContainerMgr::Shutdown()
	{
		for (std::pair<const ContainerId, Container*>& pair : m_containerMap)
		{
			delete pair.second;
		}

		m_containerMap.clear();
	}

	Container* ContainerMgr::Create(const char* seed)
	{
		//first make a new container id
		Core::Sid startSid = SID(seed);

		ContainerId cid(startSid);

		const int MAX_ITERATION = 10000;
		int ii = 0;
		std::map<ContainerId, Container*>::const_iterator it = m_containerMap.find(cid);
		while (it != m_containerMap.cend() && ii < MAX_ITERATION) //boo caca
		{
			++ii;
			++startSid;
			ContainerId cid(startSid);
			it = m_containerMap.find(cid);
		}

		if (ii >= MAX_ITERATION)
			return nullptr;

		//now create the container
		Container* pContainer = new Container(cid);
		m_containerMap[cid] = pContainer;

		return pContainer;
	}

	Container* ContainerMgr::Get(ContainerId cid)
	{
		std::map<ContainerId, Container*>::const_iterator it = m_containerMap.find(cid);
		if (it == m_containerMap.cend())
			return nullptr;

		return it->second;
	}

	bool ContainerMgr::Load(ContainerId cid)
	{
		return true;
	}

	bool ContainerMgr::Save(ContainerId cid)
	{
		return true;
	}

	std::string ContainerMgr::MakeFilename(ContainerId cid)
	{
		std::string cidStr = cid.ToString();
		std::string firstSubFolder = cidStr.substr(0, 2);
		std::string secondSubFolder = cidStr.substr(2, 2);
		std::string thirdSubFolder = cidStr.substr(4, 2);
		std::string filename = m_root + firstSubFolder + "\\" + secondSubFolder + "\\" + thirdSubFolder + "\\" + cidStr;
		return filename;
	}
}
