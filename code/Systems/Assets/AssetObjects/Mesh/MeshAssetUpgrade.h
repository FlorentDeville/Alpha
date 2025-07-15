/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Systems
{
	class MeshAsset_v1;
	class MeshAsset_v2;
	void UpgradeMeshAssetV1ToV2(const MeshAsset_v1* pMeshV1, MeshAsset_v2* pMeshV2);
}
