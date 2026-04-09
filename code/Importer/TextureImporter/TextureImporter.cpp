/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Importer/TextureImporter/TextureImporter.h"

#include "Systems/Assets/AssetObjects/Texture/CubemapAsset.h"
#include "Systems/Assets/AssetObjects/Texture/Texture2DAsset.h"

#include "DirectXTex/DirectXTex.h"

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <wincodec.h>
#include <Windows.h>

namespace Importer
{
    //DirectXTex only initialize WIC factory once. But since I init/uninit the com factory every import, the second import has a broken
    //wic factory. So I use my own wic factory instead and pass it to DirectXTex.
    bool InitializeWICFactory()
    {
        void* pFactory = nullptr;

        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory2,
            nullptr,
            CLSCTX_INPROC_SERVER,
            __uuidof(IWICImagingFactory2),
            &pFactory
        );

        if (SUCCEEDED(hr))
        {
            // WIC2 is available on Windows 10, Windows 8.x, and Windows 7 SP1 with KB 2670838 installed
            return true;
        }
        else
        {
            hr = CoCreateInstance(
                CLSID_WICImagingFactory1,
                nullptr,
                CLSCTX_INPROC_SERVER,
                __uuidof(IWICImagingFactory),
                &pFactory
            );
            return SUCCEEDED(hr) ? true : false;
        }

        DirectX::SetWICFactory(static_cast<IWICImagingFactory*>(pFactory));
    }

    ID3D11Device* CreateDx11Device()
    {
        UINT createDeviceFlags = 0;
#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //#ifdef _DEBUG

        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

        ID3D11Device* pDevice = nullptr;
        D3D_FEATURE_LEVEL supportedFeatureLevel;
        HRESULT hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            createDeviceFlags,
            featureLevels,
            1,
            D3D11_SDK_VERSION,
            &pDevice,
            &supportedFeatureLevel,
            nullptr
        );

        if (FAILED(hr))
            return nullptr;

        return pDevice;
    }

    struct ScopeComInitialize
    {
        ScopeComInitialize()
        {
            m_res = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        }

        ~ScopeComInitialize()
        {
            if(SUCCEEDED(m_res))
                CoUninitialize();
        }

        HRESULT m_res;
    };

    TextureImporter::Result::Result(Code code)
        : m_code(code)
        , m_msg()
    { }

    TextureImporter::Result::Result(Code code, const char* pMsg, ...)
    {
        m_code = code;
        va_list args;
        va_start(args, pMsg);


        int size = snprintf(nullptr, 0, pMsg, args);
        m_msg.resize(size + 1, '\0');

        snprintf(&m_msg[0], m_msg.size(), pMsg, args);
        va_end(args);
    }

    bool TextureImporter::Result::IsSuccess() const
    {
        return m_code == Code::Ok;
    }

    bool TextureImporter::Result::IsFailure() const
    {
        return !IsSuccess();
    }

    TextureImporter::Result TextureImporter::Import(const std::string& sourceFilename, Systems::Texture2DAsset* pTexture) const
    {
        ScopeComInitialize comJanitor;
        if (FAILED(comJanitor.m_res))
            return Result(Result::ComError, "Failed to initialize COM (%08X)", static_cast<unsigned int>(comJanitor.m_res));

        if (!InitializeWICFactory())
            return Result::WicFactoryError;

        std::wstring wideSourceFilename = std::wstring(sourceFilename.begin(), sourceFilename.end());
        DirectX::ScratchImage image;
        HRESULT hr = DirectX::LoadFromWICFile(wideSourceFilename.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
        if (FAILED(hr))
            return Result(Result::LoadingFailed, "Failed to load texture %s (%08X)", sourceFilename.c_str(), static_cast<unsigned int>(hr));

        DirectX::ScratchImage timage;
        hr = DirectX::GenerateMipMaps(*image.GetImage(0, 0, 0), DirectX::TEX_FILTER_DEFAULT, 0, timage);
        if (FAILED(hr))
            return Result(Result::MipMapFailed, "Failed to generate mip maps (%08X)", static_cast<unsigned int>(hr));

        image.Release();

        //create dx11 device for compression
        ID3D11Device* pDx11Device = CreateDx11Device();
        if(!pDx11Device)
            return Result(Result::Dx11DeviceFailed, "Failed to initialize dx11 device (%08X)", static_cast<unsigned int>(hr));

        hr = DirectX::Compress(pDx11Device, timage.GetImages(), timage.GetImageCount(), timage.GetMetadata(),
            DXGI_FORMAT_BC7_UNORM_SRGB, DirectX::TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT,
            image);

        pDx11Device->Release();

        if (FAILED(hr))
            return Result(Result::CompressionFailed, "Failed to compress textture (%08X)", static_cast<unsigned int>(hr));

        DirectX::Blob textureBlob;
        hr = DirectX::SaveToDDSMemory(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::DDS_FLAGS_NONE, textureBlob);
        if (FAILED(hr))
            return Result(Result::WriteDDSFailed, "Failed to write DDS to memory (%08X)", static_cast<unsigned int>(hr));

        uint32_t width = static_cast<uint32_t>(image.GetMetadata().width);
        uint32_t height = static_cast<uint32_t>(image.GetMetadata().height);
        uint32_t mipCount = static_cast<uint32_t>(image.GetImageCount());

        image.Release();
        timage.Release();

        size_t blobSize = textureBlob.GetBufferSize();
        if (blobSize > UINT32_MAX)
            return Result(Result::BadSize, "Current size is %lld but maximum size is %lld", blobSize, UINT32_MAX);

        pTexture->Init(sourceFilename, textureBlob.GetConstBufferPointer(), static_cast<uint32_t>(blobSize), width, height, mipCount/*, Rendering::TextureFormat::BC7_SRGB*/);
        return Result::Ok;
    }

    TextureImporter::Result TextureImporter::ImportCubemap(const std::string sourceFilename[6], Systems::CubemapAsset* pCubemap)
    {
        ScopeComInitialize comJanitor;
        if (FAILED(comJanitor.m_res))
            return Result(Result::ComError, "Failed to initialize COM(%08X)", static_cast<unsigned int>(comJanitor.m_res));

        if (!InitializeWICFactory())
            return Result(Result::WicFactoryError, "Failed to create the wic factory.");

        const int IMAGE_COUNT = 6;

        DirectX::Image imageArray[IMAGE_COUNT];
        DirectX::ScratchImage baseImage[IMAGE_COUNT];
        for (uint32_t ii = 0; ii < IMAGE_COUNT; ++ii)
        {
            std::wstring wideSourceFilename = std::wstring(sourceFilename[ii].begin(), sourceFilename[ii].end());
            HRESULT hr = DirectX::LoadFromWICFile(wideSourceFilename.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, baseImage[ii]);
            if (FAILED(hr))
                return Result(Result::LoadingFailed, "Failed to load texture %s : (%08X)", sourceFilename[ii].c_str(), static_cast<unsigned int>(hr));

            imageArray[ii] = *baseImage[ii].GetImage(0, 0, 0);
        }

        DirectX::ScratchImage cubemapImage;
        HRESULT hr = cubemapImage.InitializeCubeFromImages(imageArray, IMAGE_COUNT);
        if (FAILED(hr))
            return Result(Result::CubemapCreationFailed, "Failed to create cubemap : (%08X)", static_cast<unsigned int>(hr));

        DirectX::ScratchImage mipMappedImage;
        hr = DirectX::GenerateMipMaps(cubemapImage.GetImages(), cubemapImage.GetImageCount(), cubemapImage.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipMappedImage);
        if (FAILED(hr))
            return Result(Result::MipMapFailed, "Failed to create mipmaps : (%08X)", static_cast<unsigned int>(hr));

        //create dx11 device for compression
        ID3D11Device* pDx11Device = CreateDx11Device();
        if (!pDx11Device)
            return Result(Result::Dx11DeviceFailed, "Failed to initialize dx11 device (%08X)", static_cast<unsigned int>(hr));

        DirectX::ScratchImage compressedImage;
        hr = DirectX::Compress(pDx11Device, mipMappedImage.GetImages(), mipMappedImage.GetImageCount(), mipMappedImage.GetMetadata(),
            DXGI_FORMAT_BC7_UNORM_SRGB, DirectX::TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT,
            compressedImage);

        DirectX::Blob textureBlob;
        hr = DirectX::SaveToDDSMemory(compressedImage.GetImages(), compressedImage.GetImageCount(), compressedImage.GetMetadata(), DirectX::DDS_FLAGS_NONE, textureBlob);
        if (FAILED(hr))
            return Result(Result::WriteDDSFailed, "Failed to write texture to DDS : (%08X)", static_cast<unsigned int>(hr));

        size_t blobSize = textureBlob.GetBufferSize();
        if (blobSize > UINT32_MAX)
            return Result(Result::BadSize, "Size (%lld) above the maximum allowed %lld", blobSize, UINT32_MAX);

        bool res = pCubemap->Init(sourceFilename, textureBlob.GetConstBufferPointer(), static_cast<uint32_t>(blobSize));
        if (!res)
            return Result(Result::InitFailed, "Failed to initialize cubemap asset");

#ifdef _DEBUG
        const wchar_t* outputFilename = L"C:\\tmp\\cubemap.dds";
        DirectX::SaveToDDSFile(compressedImage.GetImages(), compressedImage.GetImageCount(), compressedImage.GetMetadata(), DirectX::DDS_FLAGS_NONE, outputFilename);
#endif // #ifdef _DEBUG

        for (DirectX::ScratchImage& image : baseImage)
            image.Release();

        cubemapImage.Release();
        mipMappedImage.Release();
        compressedImage.Release();

        return Result::Ok;
    }

    TextureImporter::Result TextureImporter::Export(const std::string& outputFilename, const Systems::Texture2DAsset* pTexture)
    {
        DirectX::TexMetadata metadata;
        DirectX::ScratchImage image;
        const Core::Blob& blob = pTexture->GetBlob();
        HRESULT res = DirectX::LoadFromDDSMemory(blob.GetData(), blob.GetSize(), DirectX::DDS_FLAGS_NONE, &metadata, image);
        if (FAILED(res))
            return Result(Result::LoadingFailed, "Failed to write texture to DDS : (%08X)", static_cast<unsigned int>(res));
        
        std::wstring wideOutputFilename = std::wstring(outputFilename.begin(), outputFilename.end());
        res = DirectX::SaveToDDSFile(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::DDS_FLAGS_NONE, wideOutputFilename.c_str());

        if (FAILED(res))
            return Result(Result::SaveToFileFailed, "Failed to write texture to DDS : (%08X)", static_cast<unsigned int>(res));

        return Result::Ok;
    }

    TextureImporter::Result TextureImporter::Export(const std::string& outputFilename, const Systems::CubemapAsset* pCubemap)
    {
        DirectX::TexMetadata metadata;
        DirectX::ScratchImage image;
        const Core::Blob& blob = pCubemap->GetBlob();
        HRESULT res = DirectX::LoadFromDDSMemory(blob.GetData(), blob.GetSize(), DirectX::DDS_FLAGS_NONE, &metadata, image);
        if (FAILED(res))
            return Result(Result::LoadingFailed, "Failed to write texture to DDS : (%08X)", static_cast<unsigned int>(res));

        std::wstring wideOutputFilename = std::wstring(outputFilename.begin(), outputFilename.end());
        res = DirectX::SaveToDDSFile(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::DDS_FLAGS_NONE, wideOutputFilename.c_str());

        if (FAILED(res))
            return Result(Result::SaveToFileFailed, "Failed to write texture to DDS : (%08X)", static_cast<unsigned int>(res));

        return Result::Ok;
    }
}
