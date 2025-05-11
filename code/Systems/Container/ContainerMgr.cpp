/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Container/ContainerMgr.h"

#include "Core/Json/JsonDeserializer.h"
#include "Core/Json/JsonObject.h"
#include "Core/Json/JsonSerializer.h"
#include "Core/Sid/Sid.h"

#include "Systems/Container/Container.h"
#include "Systems/Container/ContainerId.h"
#include "Systems/Serialization/ContainerJsonDeserializer.h"
#include "Systems/Serialization/ContainerJsonSerializer.h"

#include <filesystem>
#include <fstream>

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

	Container* ContainerMgr::CreateContainer(const char* seed)
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

	bool ContainerMgr::DeleteContainer(ContainerId cid)
	{
		//delete the container file
		std::string rawFilename = MakeFilename(cid);
		std::filesystem::path filename(rawFilename);
		std::error_code error;
		std::filesystem::remove(filename, error);

		//delete empty folders
		std::filesystem::path root(m_root);
		if(m_root.back() == '\\')
			root = root.parent_path();

		std::filesystem::path parentPath = filename.parent_path();
		while (parentPath != filename && parentPath != root)
		{
			bool deleted = std::filesystem::remove(parentPath, error);
			if (!deleted)
				break;

			filename = parentPath;
			parentPath = filename.parent_path();
		}

		//delete the container
		Container* pContainer = GetContainer(cid);
		if (pContainer)
			delete pContainer;

		//remove the container from the manager
		m_containerMap.erase(cid);

		return true;
	}

	Container* ContainerMgr::GetContainer(ContainerId cid)
	{
		std::map<ContainerId, Container*>::const_iterator it = m_containerMap.find(cid);
		if (it == m_containerMap.cend())
			return nullptr;

		return it->second;
	}

	Container* ContainerMgr::LoadContainer(ContainerId cid)
	{
		std::string fileContent;

		//first let's read the file
		{
			std::string filename = MakeFilename(cid);

			FILE* pFile = nullptr;
			fopen_s(&pFile, filename.c_str(), "rb");
			if (!pFile)
				return nullptr;

			fseek(pFile, 0, SEEK_END);
			size_t fileSize = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);

			fileContent.resize(fileSize);
			size_t sizeRead = fread(&fileContent[0], sizeof(char), fileSize, pFile);
			fclose(pFile);

			if (sizeRead != fileSize)
				return nullptr;
		}

		Core::JsonObject json;
		Core::JsonDeserializer deser;
		bool res = deser.Deserialize(fileContent, json);
		if (!res)
			return nullptr;

		Container* pContainer = new Container();
		ContainerJsonDeserializer containerDeser;
		res = containerDeser.Deserialize(json, *pContainer);
		if (!res)
			return nullptr;

		m_containerMap[pContainer->GetId()] = pContainer;
		return pContainer;
	}

	bool ContainerMgr::SaveContainer(ContainerId cid)
	{
		Container* pContainer = GetContainer(cid);
		if (!pContainer)
			return false;

		Core::JsonObject json;
		ContainerJsonSerializer ser;
		bool res = ser.Serialize(pContainer, json);
		if (!res)
			return false;

		std::string strContainer;
		Core::JsonSerializer jsonSer;
		res = jsonSer.Serialize(json, strContainer);
		if (!res)
			return false;

		std::string directory = MakeDirectory(cid);
		std::filesystem::create_directories(directory);

		std::string filename = MakeFilename(cid);
		std::ofstream fileStream(filename);
		fileStream << strContainer;
		fileStream.close();

		return true;
	}

	std::string ContainerMgr::MakeDirectory(ContainerId cid)
	{
		std::string cidStr = cid.ToString();
		std::string firstSubFolder = cidStr.substr(2, 2);
		std::string secondSubFolder = cidStr.substr(4, 2);
		std::string thirdSubFolder = cidStr.substr(6, 2);
		std::string directory = m_root + firstSubFolder + "\\" + secondSubFolder + "\\" + thirdSubFolder + "\\";
		return directory;
	}

	std::string ContainerMgr::MakeFilename(ContainerId cid)
	{
		std::string cidStr = cid.ToString();
		return MakeDirectory(cid) + cidStr;
	}
}
