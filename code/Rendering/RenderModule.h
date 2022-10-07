/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <cstdint>
#include <map>

#include <d3d12.h>
#include <DirectXMath.h>

#include "Core/Singleton.h"
#include "Core/Resource/ResourceMgr.h"

#include "Rendering/DescriptorHeap.h"
#include "Rendering/Font/Font.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/RootSignature/RootSignatureId.h"
#include "ShaderId.h"

struct IDXGIAdapter4;
struct IDXGISwapChain4;
struct ID3D12CommandQueue;
struct ID3D12DescriptorHeap;
struct ID3D12Device2;

class CommandQueue;
class Mesh;
namespace Rendering
{
	class Material;
	class RenderTarget;
}

using FontId = size_t;

class RenderModule : public Core::Singleton<RenderModule>
{
public:
	using TextureMgr = ResourceMgr<Texture, TextureId>;

	RenderModule();
	~RenderModule();

	void Init(HWND hWindow, const DirectX::XMUINT2& gameResolution, const DirectX::XMUINT2& mainResolution);
	void Release();

	void PreRender();
	void PostRender();

	void BeginMainScene();
	void EndMainScene();

	void BindMaterial(const Rendering::Material& material, const DirectX::XMMATRIX& wvp);
	void RenderMesh(const Mesh& mesh);

	void ExecuteRenderCommand();

	void SetConstantBuffer(int32_t registerId, int32_t sizeInBytes, void* pData, int32_t offset);

	int GetNumFrames() const;
	const DirectX::XMUINT2& GetGameResolution() const;

	void InitialiseFont(FontId fontId, PipelineStateId psoId, int maxCharacterCount);

	//uiPos : origin : top left, range : [0, pixel screen size]
	void PrepareRenderText(const std::string& text, FontId fontId, const DirectX::XMFLOAT3& uiPos, const DirectX::XMFLOAT2& scale, const DirectX::XMUINT4& scissor);
	void RenderAllText();

	TextureId GetGameRenderTargetTextureId() const;

	void ChangeMainResolution(const DirectX::XMUINT2& size);

	Rendering::RenderTarget* CreateRenderTarget(int width, int height);
	Rendering::DescriptorHeap* CreateRTVHeap();
	Rendering::DescriptorHeap* CreateDSVHeap();

	TextureId GetRenderTargetTextureId(const Rendering::RenderTarget* pRenderTarget) const;

	//Temp functions
	CommandQueue* GetRenderCommandQueue();
	CommandQueue* GetCopyCommandQueue();
	ID3D12GraphicsCommandList2* GetRenderCommandList();
	ID3D12Device2* GetDevice();

	static void ReportLiveObject();

	TextureMgr& GetTextureMgr();
	ResourceMgr<Font, FontId>& GetFontMgr();

private:
	DirectX::XMUINT2 m_gameResolution;
	DirectX::XMUINT2 m_mainResolution;

	static const int m_numFrames = 3;

	ID3D12Device2* m_pDevice;

#if defined(_DEBUG)
	ID3D12Debug* m_pDebugInterface;
#endif

	IDXGISwapChain4* m_pSwapChain;
	
	Rendering::DescriptorHeap* m_pRTVHeap;
	Rendering::DescriptorHeap* m_pDSVHeap;

	CommandQueue* m_pRenderCommandQueue;
	ID3D12GraphicsCommandList2* m_pRenderCommandList;

	CommandQueue* m_pCopyCommandQueue;

	bool m_allowTearing;
	bool m_vSync; //True to limit to 60 fps

	struct FontRenderInfo
	{
		//One vertex buffer per frame
		ID3D12Resource* m_textVertexBuffer[m_numFrames];
		char* m_textVBGPUAddress[m_numFrames];
		D3D12_VERTEX_BUFFER_VIEW m_textVertexBufferView[m_numFrames];

		int m_bufferSize;		//the size of the vertex buffer
		int m_characterCount;	//the number of character to render
		PipelineStateId m_psoId;
	};
	std::map<FontId, FontRenderInfo> m_fontVertexBuffers;	//one font info per font used

	float m_clearColor[4];

	TextureMgr m_textureMgr;
	ResourceMgr<Font, FontId> m_fontMgr;

public:
	UINT m_currentBackBufferIndex;
	Rendering::RenderTarget* m_gameRenderTarget;

private:
	ID3D12Resource* m_pBackBuffers[m_numFrames];
	ID3D12Resource* m_pMainDepthBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_mainRTV[m_numFrames];
	D3D12_CPU_DESCRIPTOR_HANDLE m_mainDSV;
	D3D12_VIEWPORT m_mainViewport;
	D3D12_RECT m_mainScissorRect;
	
	void CreateDevice(IDXGIAdapter4* pAdapter);

	IDXGIAdapter4* GetAdapter(bool useWarp);
	void EnableDebugLayer();
	void CheckTearingSupport();

	void CreateSwapChain(HWND hWnd, ID3D12CommandQueue* pCommandQueue, uint32_t width, uint32_t height, uint32_t bufferCount);
	void UpdateRenderTargetViews();

	void ResizeSwapChain(uint32_t width, uint32_t height);
	void ResizeDepthBuffer(uint32_t width, uint32_t height, ID3D12Resource** pResource, D3D12_CPU_DESCRIPTOR_HANDLE dsv);
};
