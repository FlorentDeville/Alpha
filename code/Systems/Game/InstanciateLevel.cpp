/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/InstanciateLevel.h"

#include "Systems/Assets/AssetObjects/Level/LevelAsset.h"
#include "Systems/Objects/GameObject.h"

namespace Systems
{
	void InstanciateLevel(LevelAsset* pLevel)
	{
		Core::Array<GameObject*>& objectArray = pLevel->GetGameObjectsArray();
		for (GameObject* pObject : objectArray)
			pObject->OnStart();
	}

	void DeleteInstanciatedLevel(LevelAsset* pLevel)
	{
		Core::Array<GameObject*>& objectArray = pLevel->GetGameObjectsArray();
		for (GameObject* pObject : objectArray)
			pObject->OnDestroy();
	}
}
