/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Importer/TextureImporter/TextureImporter.h"

#include "DirectXTex/DirectXTex.h"
#include <Windows.h>

namespace Importer
{
    struct ScopeComInitialize
    {
        ScopeComInitialize()
        {
            m_res = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        }

        ~ScopeComInitialize()
        {
            CoUninitialize();
        }

        HRESULT m_res;
    };

	bool TextureImporter::Import(const std::string& sourceFilename, Systems::TextureAsset* pTexture)
	{
        ScopeComInitialize comJanitor;
        if (FAILED(comJanitor.m_res))
        {
            //wprintf(L"Failed to initialize COM (%08X)\n", static_cast<unsigned int>(comJanitor.m_res));
            return false;
        }

        std::wstring wideSourceFilename = std::wstring(sourceFilename.begin(), sourceFilename.end());
        DirectX::ScratchImage image;
        HRESULT hr = DirectX::LoadFromWICFile(wideSourceFilename.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
        if (FAILED(hr))
        {
            //wprintf(L"Failed to load texture.png (%08X)\n", static_cast<unsigned int>(hr));
            return false;
        }

        DirectX::ScratchImage timage;
        hr = DirectX::GenerateMipMaps(*image.GetImage(0, 0, 0), DirectX::TEX_FILTER_DEFAULT, 0, timage);
        if (FAILED(hr))
        {
            //wprintf(L"Failed to generate mipmaps (%08X)\n", static_cast<unsigned int>(hr));
            return false;
        }

        image.Release();

        hr = DirectX::Compress(timage.GetImages(), timage.GetImageCount(), timage.GetMetadata(),
            DXGI_FORMAT_BC7_UNORM, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT,
            image);
        if (FAILED(hr))
        {
            //wprintf(L"Failed to compress texture (%08X)\n", static_cast<unsigned int>(hr));
            return false;
        }

        DirectX::Blob textureBlob;
        hr = DirectX::SaveToDDSMemory(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::DDS_FLAGS_NONE, textureBlob);
        if (FAILED(hr))
        {
            //wprintf(L"Failed to write texture to DDS (%08X)\n", static_cast<unsigned int>(hr));
            return false;
        }

        pTexture->Init(sourceFilename, textureBlob.GetConstBufferPointer(), textureBlob.GetBufferSize());
        return true;
	}
}
