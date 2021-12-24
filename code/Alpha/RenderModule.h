/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <cstdint>

#include <d3d12.h>

struct IDXGIAdapter4;
struct IDXGISwapChain4;
struct ID3D12CommandQueue;
struct ID3D12DescriptorHeap;
struct ID3D12Device2;

class CommandQueue;

class RenderModule
{
public:
	RenderModule();
	~RenderModule();

	void Init(HWND hWindow, uint32_t width, uint32_t height);
	void Shutdown();

	void PreRender();
	void Present();

	void ResizeSwapChain(uint32_t width, uint32_t height);
	void ResizeDepthBuffer(uint32_t width, uint32_t height);

	//Temp functions
	CommandQueue* GetRenderCommandQueue();
	CommandQueue* GetCopyCommandQueue();
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTV();
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSV();
	ID3D12Device2* GetDevice();
	void SetContentLoaded();

	static void ReportLiveObject();

private:
	static const int m_numFrames = 3;

	ID3D12Device2* m_pDevice;

#if defined(_DEBUG)
	ID3D12Debug* m_pDebugInterface;
#endif

public:
	IDXGISwapChain4* m_pSwapChain;
	
private:
	ID3D12DescriptorHeap* m_pRTVDescriptorHeap;	// render target view descriptor heap
	ID3D12DescriptorHeap* m_pDSVDescriptorHeap; //depth stencil view descriptor heap

	UINT m_RTVDescriptorSize;

	CommandQueue* m_pRenderCommandQueue;
	CommandQueue* m_pCopyCommandQueue;

	bool m_allowTearing;
	bool m_vSync;

	//TEMP
public:
	UINT m_currentBackBufferIndex;


	ID3D12Resource* m_pBackBuffers[m_numFrames];
	ID3D12Resource* m_pDepthBuffer;

private:
	bool m_contentLoaded;

	void CreateDevice(IDXGIAdapter4* pAdapter);

	IDXGIAdapter4* GetAdapter(bool useWarp);
	void EnableDebugLayer();
	void CheckTearingSupport();

	void CreateSwapChain(HWND hWnd, ID3D12CommandQueue* pCommandQueue, uint32_t width, uint32_t height, uint32_t bufferCount);
	void CreateRTVDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
	void CreateDSVDescriptorHeap();
	void UpdateRenderTargetViews();
};
