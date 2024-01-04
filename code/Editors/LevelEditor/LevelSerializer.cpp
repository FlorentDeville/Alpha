/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/LevelSerializer.h"

#include "Editors/LevelEditor/Component.h"
#include "Editors/LevelEditor/SceneTree/Entity.h"
#include "Editors/LevelEditor/SceneTree/SceneTree.h"

#include "Systems/Assets/Asset.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"

namespace Editors
{
	bool SerializeComponent(rapidjson::Value& jsonComponentArray, rapidjson::Document::AllocatorType& alloc, const Component* pComponent)
	{
		rapidjson::Value jsonComponent(rapidjson::kObjectType);

		jsonComponent.AddMember("name", rapidjson::Value(pComponent->GetName().c_str(), alloc), alloc);
		
		rapidjson::Value jsonPropertyArray(rapidjson::kArrayType);
		for (int ii = 0; ii < pComponent->GetPropertyCount(); ++ii)
		{
			const Property* pProperty = pComponent->GetProperty(ii);

			rapidjson::Value jsonProperty(rapidjson::kObjectType);
			jsonProperty.AddMember("name", rapidjson::Value(pProperty->GetName().c_str(), alloc), alloc);

			const char* propertyType = PropertyTypeToString(pProperty->GetType());
			jsonProperty.AddMember("type", rapidjson::Value(propertyType, alloc), alloc);

			switch (pProperty->GetType())
			{
			case kInt:
				jsonProperty.AddMember("value", pProperty->GetValue<int32_t>(), alloc);
				break;

			case kFloat:
				jsonProperty.AddMember("value", pProperty->GetValue<float>(), alloc);
				break;

			case kString:
				jsonProperty.AddMember("value", rapidjson::Value(pProperty->GetValue<std::string>().c_str(), alloc), alloc);
				break;

			case kAssetMesh:
			case kAssetMaterial:
			{
				const Systems::AssetId& id = pProperty->GetValue<Systems::AssetId>();
				jsonProperty.AddMember("value", id.ToUint64(), alloc);
			}
			break;

			case kVec4f:
			{
				const Core::Vec4f& vec = pProperty->GetValue<Core::Vec4f>();
				rapidjson::Value jsonVecArray(rapidjson::kArrayType);
				jsonVecArray.PushBack(vec.GetX(), alloc);
				jsonVecArray.PushBack(vec.GetY(), alloc);
				jsonVecArray.PushBack(vec.GetZ(), alloc);
				jsonVecArray.PushBack(vec.GetW(), alloc);

				jsonProperty.AddMember("value", jsonVecArray, alloc);
			}
			break;

			case kMat44f:
			{
				const Core::Mat44f& mat = pProperty->GetValue<Core::Mat44f>();
				rapidjson::Value jsonMatArray(rapidjson::kArrayType);
				for (int jj = 0; jj < 4; ++jj)
				{
					for (int kk = 0; kk < 4; ++kk)
					{
						jsonMatArray.PushBack(mat.Get(jj, kk), alloc);
					}
				}
				
				jsonProperty.AddMember("value", jsonMatArray, alloc);
			}
			break;

			default:
				assert(false);
				break;
			}

			jsonPropertyArray.PushBack(jsonProperty, alloc);
		}

		jsonComponent.AddMember("properties", jsonPropertyArray, alloc);
		jsonComponentArray.PushBack(jsonComponent, alloc);

		return true;
	}

	bool DeserializeComponent(const rapidjson::Value& jsonComponent, Component** ppComponent)
	{
		const char* componentName = jsonComponent["name"].GetString();
		*ppComponent = new Component(componentName);

		const rapidjson::Value& jsonPropertyArray = jsonComponent["properties"];
		for (const rapidjson::Value& jsonProperty : jsonPropertyArray.GetArray())
		{
			const char* propertyName = jsonProperty["name"].GetString();
			const char* strPropertyType = jsonProperty["type"].GetString();
			PropertyType type = StringToPropertyType(strPropertyType);
			
			Property* pNewProperty = new Property(propertyName);
			switch (type)
			{
			case kAssetMaterial:
			{
				uint64_t id = jsonProperty["value"].GetUint64();
				pNewProperty->SetMaterialAssetId(Systems::AssetId(id));
			}
			break;

			case kAssetMesh:
			{
				uint64_t id = jsonProperty["value"].GetUint64();
				pNewProperty->SetMeshAssetId(Systems::AssetId(id));
			}
			break;

			case kMat44f:
			{
				Core::Mat44f mat;
				const rapidjson::Value& jsonMatArray = jsonProperty["value"];
				for (int ii = 0; ii < 4; ++ii)
				{
					int offset = ii * 4;
					for (int jj = 0; jj < 4; ++jj)
					{
						int index = offset + jj;
						float v = jsonMatArray[index].GetFloat();
						mat.Set(ii, jj, v);
					}
				}

				pNewProperty->SetMatrix(mat);
			}
			break;

			default:
				assert(false);
				break;

			}

			(*ppComponent)->AddProperty(pNewProperty);
		}

		return true;
	}

	bool SerializeEntity(rapidjson::Value& jsonEntityArray, rapidjson::Document::AllocatorType& alloc, const Entity* pEntity)
	{
		rapidjson::Value entityJson(rapidjson::kObjectType);

		entityJson.AddMember("type", rapidjson::Value("entity"), alloc);

		const int GUID_BUFFER_LENGTH = 37;
		char guidBuffer[GUID_BUFFER_LENGTH];
		pEntity->GetConstGuid().ToString(guidBuffer, GUID_BUFFER_LENGTH);
		entityJson.AddMember("guid", rapidjson::Value(guidBuffer, alloc), alloc);
		
		entityJson.AddMember("name", rapidjson::Value(pEntity->GetName().c_str(), alloc), alloc);

		rapidjson::Value jsonComponentArray(rapidjson::kArrayType);
		for (int ii = 0; ii < pEntity->GetComponentCount(); ++ii)
		{
			const Component* pComponent = pEntity->GetComponent(ii);
			bool res = SerializeComponent(jsonComponentArray, alloc, pComponent);
			if (!res)
				return false;
		}

		entityJson.AddMember("components", jsonComponentArray, alloc);
		
		//go recursively to save children
		rapidjson::Value jsonChildren(rapidjson::kArrayType);

		const std::vector<Node*>& children = pEntity->GetConstChildren();
		for (const Node* pChildNode : children)
		{
			const Entity* pChildEntity = pChildNode->ToConstEntity();
			bool res = SerializeEntity(jsonChildren, alloc, pChildEntity);
			if (!res)
				return false;
		}

		entityJson.AddMember("children", jsonChildren, alloc);

		jsonEntityArray.PushBack(entityJson, alloc);

		return true;
	}

	bool DeserializeEntity(const rapidjson::Value& jsonEntity, SceneTree* pSceneTree, const Os::Guid& parentGuid)
	{
		std::string strGuid = jsonEntity["guid"].GetString();
		Os::Guid guid(strGuid.c_str());

		std::string name = jsonEntity["name"].GetString();

		Entity* pEntity = new Entity(name, guid);
		pSceneTree->AddNode(pEntity, parentGuid);

		//load components
		const rapidjson::Value& jsonComponentArray = jsonEntity["components"];
		for (const rapidjson::Value& jsonComponent : jsonComponentArray.GetArray())
		{
			Component* pComponent = nullptr;
			DeserializeComponent(jsonComponent, &pComponent);

			pEntity->AddComponent(pComponent);
		}

		//now load the children
		const rapidjson::Value& jsonChildrenArray = jsonEntity["children"];
		for (const rapidjson::Value& jsonChild : jsonChildrenArray.GetArray())
		{
			Entity* pChildEntity = nullptr;
			DeserializeEntity(jsonChild, pSceneTree, pEntity->GetConstGuid());
		}

		return true;
	}

	bool LevelSerializer::Serialize(const Systems::Asset& asset, const std::string& levelName, const SceneTree* pSceneTree)
	{
		rapidjson::Document document;

		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

		document.SetObject();

		//write the name
		rapidjson::Value valueName(levelName.c_str(), allocator);
		document.AddMember("levelname", valueName, allocator);

		const Node* pRoot = pSceneTree->GetConstRoot();
		if (!pRoot)
			return true;

		const Entity* pRootEntity = pRoot->ToConstEntity();
		if (!pRootEntity)
			return true;

		rapidjson::Value sceneTreeObj(rapidjson::Type::kArrayType);
		bool res = SerializeEntity(sceneTreeObj, allocator, pRootEntity);
		if (!res)
			return false;

		document.AddMember("scenetree", sceneTreeObj, allocator);

		const std::string& assetPath = asset.GetPath();
		FILE* pFile = nullptr;
		errno_t err = fopen_s(&pFile, assetPath.c_str(), "wb"); //this should be moved to the OsWin library
		if (err != 0)
			return false;

		const int WRITE_BUFFER_SIZE = 65536;
		char* pWriteBuffer = new char[WRITE_BUFFER_SIZE];

		rapidjson::FileWriteStream os(pFile, pWriteBuffer, WRITE_BUFFER_SIZE);
		rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);

		document.Accept(writer);

		delete[] pWriteBuffer;

		fclose(pFile);

		return true;
	}

	bool LevelSerializer::Deserialize(const Systems::Asset& asset, std::string& levelName, SceneTree* pSceneTree)
	{
		const std::string& assetPath = asset.GetPath();
		FILE* pFile = nullptr;
		errno_t err = fopen_s(&pFile, assetPath.c_str(), "rb");
		if (err != 0)
			return false;

		const int READ_BUFFER_SIZE = 65536;
		char* pReadBuffer = new char[READ_BUFFER_SIZE];

		rapidjson::FileReadStream frs(pFile, pReadBuffer, READ_BUFFER_SIZE);

		rapidjson::Document document;
		document.ParseStream(frs);

		delete[] pReadBuffer;
		fclose(pFile);

		levelName = document["levelname"].GetString();

		const rapidjson::Value& jsonSceneTree = document["scenetree"];
		if (!jsonSceneTree.IsArray())
			return false;

		const rapidjson::Value& jsonRoot = jsonSceneTree[0];

		DeserializeEntity(jsonRoot, pSceneTree, Os::Guid());

		return true;
	}
}
