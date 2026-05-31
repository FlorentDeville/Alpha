/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <cstdint>
#include <map>
#include <string>

#include <d3d12.h>
#include <DirectXMath.h>

#include "Core/Singleton.h"

#include "Rendering/Font/FontId.h"
#include "Rendering/DescriptorHeap.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/PipelineState/PipelineStateId.h"
#include "Rendering/RootSignature/RootSignatureId.h"
#include "Rendering/Shaders/ShaderId.h"
#include "Rendering/Texture/TextureId.h"

struct IDXGIAdapter4;
struct IDXGISwapChain4;
struct ID3D12CommandQueue;
struct ID3D12DescriptorHeap;
struct ID3D12Device2;

namespace Core
{
	class Float4;
	class Mat44f;
}

namespace Rendering
{
	class BufferHandle;
	class Camera;
	class CommandQueue;
	class Device;
	class LinearConstantBufferPool;
	class Material;
	class Mesh;
	class RenderTarget;
	class Texture;

	class RenderModule : public Core::Singleton<RenderModule>
	{
	public:
		RenderModule();
		~RenderModule();

		void Init(HWND hWindow, const DirectX::XMUINT2& gameResolution, const DirectX::XMUINT2& mainResolution, const std::string& binPath);
		void Release();

		void PreRender();
		void PostRender();

		void BeginMainScene();
		void EndMainScene();

		void BindMaterial(const PipelineState& pso);
		void BindMaterial(const PipelineState& pso, const RootSignature& rs);
		void BindMaterial(const PipelineState& pso, const RootSignature& rs, const DirectX::XMMATRIX& wvp);

		void BindCBuffer(uint32_t rootParamIndex, int poolIndex);

		void RenderMesh(const Rendering::Mesh& mesh);

		//Render a single quad without passing any vertex buffer The shader do everything with the vertex id.
		void RenderInstancedQuad(uint32_t count);

		//Render a single triangle without passing any vertex buffer. The shader has to do everything with the vertexId.
		//Used to render a single triangle to copy a render target on another one or for post process.
		void RenderNoBufferTriangle();

		void SetScissorRectangle(const D3D12_RECT& rect);

		void RenderPrimitiveCylinder(const Core::Mat44f& wvp, const Core::Float4& color, bool wireframe);
		void RenderPrimitiveCone(const Core::Mat44f& wvp, const Core::Float4& color, bool wireframe);
		void RenderPrimitiveTorus(const Core::Mat44f& wvp, const Core::Float4& color, bool wireframe);
		void RenderPrimitiveCube(const Core::Mat44f& wvp, const Core::Float4& color, bool wireframe);
		void RenderPrimitiveSphere(const Core::Mat44f& wvp, const Core::Float4& color, bool wireframe);

		void ExecuteRenderCommand();

		void SetConstantBuffer(int32_t registerId, int32_t sizeInBytes, const void* pData, int32_t offset);

		void SetBlendFactor(const Core::Float4& blendFactor);

		int GetNumFrames() const;
		const DirectX::XMUINT2& GetGameResolution() const;

		void InitialiseFont(Rendering::FontId fontId, Rendering::PipelineStateId psoId, int maxCharacterCount);

		//uiPos : origin : top left, range : [0, pixel screen size]
		//return the number of character written
		int PrepareRenderText(const std::string& text, Rendering::FontId fontId, const DirectX::XMFLOAT3& uiPos, const DirectX::XMFLOAT2& scale, const DirectX::XMUINT4& scissor, float nearCameraPlane, float farCameraPlane);
		void RenderAllText();

		void ChangeMainResolution(const DirectX::XMUINT2& size);

		Rendering::RenderTarget* CreateRenderTarget(int width, int height);
		Rendering::DescriptorHeap* CreateRTVHeap();
		Rendering::DescriptorHeap* CreateDSVHeap();

		Rendering::TextureId GetRenderTargetTextureId(const Rendering::RenderTarget* pRenderTarget) const;

		LinearConstantBufferPool* GetLinearCBufferPool();

		Device* GetDevice();

		//Temp functions
		CommandQueue* GetRenderCommandQueue();
		CommandQueue* GetCopyCommandQueue();
		ID3D12GraphicsCommandList2* GetRenderCommandList();
		ID3D12Device2* GetDx12Device();

		static void ReportLiveObject();

		void BindNullCubemap(uint32_t rootSigIndex);
		void BindNullTexture2D(uint32_t rootSigIndex);

	private:
		DirectX::XMUINT2 m_gameResolution;
		DirectX::XMUINT2 m_mainResolution;

		static const int m_numFrames = 3;

		Device* m_pDevice;

#if defined(_DEBUG)
		ID3D12Debug* m_pDebugInterface;
#endif

		IDXGISwapChain4* m_pSwapChain;

		Rendering::DescriptorHeap* m_pRTVHeap;
		Rendering::DescriptorHeap* m_pDSVHeap;

		Rendering::CommandQueue* m_pRenderCommandQueue;
		ID3D12GraphicsCommandList2* m_pRenderCommandList;

		Rendering::CommandQueue* m_pCopyCommandQueue;

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
			Rendering::PipelineStateId m_psoId;
		};
		std::map<Rendering::FontId, FontRenderInfo> m_fontVertexBuffers;	//one font info per font used

		float m_clearColor[4];

		//CBuffer pool being reset every frame used for PerFrame cbuffer.
		LinearConstantBufferPool* m_pLinearCBufferPool;

	public:
		UINT m_currentBackBufferIndex;

		Mesh* m_pCylinderMesh;
		Mesh* m_pConeMesh;
		Mesh* m_pTorusMesh;
		Mesh* m_pCubeMesh;
		Mesh* m_pSphereMesh;

	private:

		PipelineState* m_pBaseShapePso;
		RootSignature* m_pBaseShapeRootSig;

		PipelineState* m_pDebugWireframePso;
		RootSignature* m_pDebugWireframeRootSig;

		ID3D12Resource* m_pBackBuffers[m_numFrames];
		ID3D12Resource* m_pMainDepthBuffer;
		D3D12_CPU_DESCRIPTOR_HANDLE m_mainRTV[m_numFrames];
		D3D12_CPU_DESCRIPTOR_HANDLE m_mainDSV;
		D3D12_VIEWPORT m_mainViewport;
		D3D12_RECT m_mainScissorRect;

		Texture* m_pNullCubemap; //used to bind to a null cubemap
		Texture* m_pNullTexture2D; //used to bind to a null texture

		void EnableDebugLayer();
		void CheckTearingSupport();

		void CreateSwapChain(HWND hWnd, ID3D12CommandQueue* pCommandQueue, uint32_t width, uint32_t height, uint32_t bufferCount);
		void UpdateRenderTargetViews();

		void ResizeSwapChain(uint32_t width, uint32_t height);
		void ResizeDepthBuffer(uint32_t width, uint32_t height, ID3D12Resource** pResource, D3D12_CPU_DESCRIPTOR_HANDLE dsv);

	public:
		void RenderBaseShape(const Mesh* pMesh, const Core::Mat44f& wvp, const Core::Float4& color, bool wireframe) const;
	};
}
