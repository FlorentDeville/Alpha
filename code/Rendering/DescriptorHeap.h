/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>
#include <d3d12.h>

namespace Rendering
{
	class DescriptorHeapImpl;
	class Device;

	enum class DescriptorHeapFlag
	{
		DEFAULT,		//D3D12_DESCRIPTOR_HEAP_FLAG_NONE
		SHADER_VISIBLE	//D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
	};

	enum class DescriptorHeapType
	{
		CBV_SRV_UAV,	//const buffer, textures
		RTV,			//render target
		DSV,			//depth stencil view
		SAMPLER			//texture sampler
	};

	class DescriptorHeap
	{
	public:
		DescriptorHeap();
		~DescriptorHeap();

		void Init(Device* pDevice, DescriptorHeapFlag flag, DescriptorHeapType type, uint32_t numDescriptors);
		void Release();

		D3D12_CPU_DESCRIPTOR_HANDLE GetNewHandle();

		ID3D12DescriptorHeap* GetHeap();

	private:
		DescriptorHeapImpl* m_pImpl;
	};
}
