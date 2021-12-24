/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <d3d12.h>
#include <cstdint>

#include <queue>

class CommandQueue
{
public:
	CommandQueue(ID3D12Device2* pDevice, D3D12_COMMAND_LIST_TYPE type);
	virtual ~CommandQueue();

	ID3D12GraphicsCommandList2* GetCommandList();

	uint64_t ExecuteCommandList(ID3D12GraphicsCommandList2* pCommandList);

	uint64_t Signal();
	bool IsFenceComplete(uint64_t fenceValue);
	bool WaitForFenceValue(uint64_t fenceValue);
	void Flush();

	ID3D12CommandQueue* GetD3D12CommandQueue() const;

protected:
	ID3D12CommandAllocator* CreateCommandAllocator();
	ID3D12GraphicsCommandList2* CreateCommandList(ID3D12CommandAllocator* pAllocator);

private:
	struct CommandAllocatorEntry
	{
		uint64_t fenceValue;
		ID3D12CommandAllocator* pCommandAllocator;
	};

	using CommandAllocatorQueue = std::queue<CommandAllocatorEntry>;
	using CommandListQueue = std::queue<ID3D12GraphicsCommandList2*>;

	D3D12_COMMAND_LIST_TYPE m_commandListType;
	ID3D12Device2* m_pDevice;
	ID3D12CommandQueue* m_pCommandQueue;
	ID3D12Fence* m_pFence;
	HANDLE m_fenceEvent;
	uint64_t m_fenceValue;

	CommandAllocatorQueue m_commandAllocatorQueue;
	CommandListQueue m_commandListQueue;
};
