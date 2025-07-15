/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetObjects/Mesh/MeshAssetUpgrade.h"

#include "Systems/Assets/AssetObjects/Mesh/MeshAsset_v1.h"
#include "Systems/Assets/AssetObjects/Mesh/MeshAsset_v2.h"

namespace Systems
{
	void UpgradeMeshAssetV1ToV2(const MeshAsset_v1* pMeshV1, MeshAsset_v2* pMeshV2)
	{
		const Core::Array<Core::Vec4f>& srcPos = pMeshV1->GetPositions();
		const Core::Array<Core::Vec4f>& srcUv = pMeshV1->GetUvs();
		const Core::Array<Core::Vec4f>& srcColor = pMeshV1->GetColors();
		const Core::Array<uint16_t>& srcIndices = pMeshV1->GetIndices();

		Core::Array<float> dstPos;
		dstPos.Reserve(srcIndices.GetSize() * 3);
		for (uint32_t ii = 0; ii < srcPos.GetSize(); ++ii)
		{
			const Core::Vec4f& src = srcPos[ii];
			dstPos.PushBack(src.GetX());
			dstPos.PushBack(src.GetY());
			dstPos.PushBack(src.GetZ());
		}

		Core::Array<float> dstUv;
		dstUv.Reserve(srcIndices.GetSize() * 2);
		for (uint32_t ii = 0; ii < srcUv.GetSize(); ++ii)
		{
			const Core::Vec4f& src = srcUv[ii];
			dstUv.PushBack(src.GetX());
			dstUv.PushBack(src.GetY());
		}

		Core::Array<float> dstColor;
		dstPos.Reserve(srcIndices.GetSize() * 4);
		for (uint32_t ii = 0; ii < srcColor.GetSize(); ++ii)
		{
			const Core::Vec4f& src = srcColor[ii];
			dstColor.PushBack(src.GetX());
			dstColor.PushBack(src.GetY());
			dstColor.PushBack(src.GetZ());
			dstColor.PushBack(src.GetW());
		}

		Core::Array<uint16_t> dstIndices;
		dstIndices.Reserve(srcIndices.GetSize());
		for (uint32_t ii = 0; ii < srcIndices.GetSize(); ++ii)
		{
			dstIndices.PushBack(srcIndices[ii]);
		}

		Core::Array<float> normals;
		normals.Resize(dstIndices.GetSize() * 3, 0);
		pMeshV2->Init(pMeshV1->GetSourceFile(), std::move(dstPos), std::move(dstUv), std::move(dstColor), std::move(normals), std::move(dstIndices));
	}
}
