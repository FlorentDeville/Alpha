/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#define NOMINMAX

#include "CommandQueue.h"

#include "Helper.h"

#include <assert.h>
#include <chrono>

CommandQueue::CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device2> pDevice, D3D12_COMMAND_LIST_TYPE type)
	: m_fenceValue(0)
	, m_commandListType(type)
	, m_pDevice(pDevice)
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    HRESULT res = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pCommandQueue));
    ThrowIfFailed(res);

    res = m_pDevice->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
    ThrowIfFailed(res);

    m_fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(m_fenceEvent && "Failed to create fence event handle.");
}

CommandQueue::~CommandQueue()
{
    ::CloseHandle(m_fenceEvent);
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CommandQueue::GetCommandList()
{
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList;

    if (!m_commandAllocatorQueue.empty() && IsFenceComplete(m_commandAllocatorQueue.front().fenceValue))
    {
        commandAllocator = m_commandAllocatorQueue.front().commandAllocator;
        m_commandAllocatorQueue.pop();

        ThrowIfFailed(commandAllocator->Reset());
    }
    else
    {
        commandAllocator = CreateCommandAllocator();
    }

    if (!m_commandListQueue.empty())
    {
        commandList = m_commandListQueue.front();
        m_commandListQueue.pop();

        ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));
    }
    else
    {
        commandList = CreateCommandList(commandAllocator);
    }

    // Associate the command allocator with the command list so that it can be
    // retrieved when the command list is executed.
    ThrowIfFailed(commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get()));

    return commandList;
}

uint64_t CommandQueue::ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> pCommandList)
{
    pCommandList->Close();

    ID3D12CommandAllocator* pCommandAllocator = nullptr;
    UINT dataSize = sizeof(pCommandAllocator);
    ThrowIfFailed(pCommandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &pCommandAllocator));

    ID3D12CommandList* const ppCommandLists[] = {
        pCommandList.Get()
    };

    m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);
    uint64_t fenceValue = Signal();

    m_commandAllocatorQueue.emplace(CommandAllocatorEntry{ fenceValue, pCommandAllocator });
    m_commandListQueue.push(pCommandList);

    // The ownership of the command allocator has been transferred to the ComPtr
    // in the command allocator queue. It is safe to release the reference 
    // in this temporary COM pointer here.
    pCommandAllocator->Release();

    return fenceValue;
}

uint64_t CommandQueue::Signal()
{
    uint64_t fenceValueForSignal = ++m_fenceValue;
    ThrowIfFailed(m_pCommandQueue->Signal(m_pFence.Get(), fenceValueForSignal));

    return fenceValueForSignal;
}

bool CommandQueue::IsFenceComplete(uint64_t fenceValue)
{
    if (m_pFence->GetCompletedValue() < fenceValue)
        return false;

    return true;
}

bool CommandQueue::WaitForFenceValue(uint64_t fenceValue)
{
    if (m_pFence->GetCompletedValue() < fenceValue)
    {
        ThrowIfFailed(m_pFence->SetEventOnCompletion(fenceValue, m_fenceEvent));

        std::chrono::milliseconds duration = std::chrono::milliseconds::max();
        ::WaitForSingleObject(m_fenceEvent, static_cast<DWORD>(duration.count()));
    }

    return true;
}

void CommandQueue::Flush()
{
    uint64_t fenceValueForSignal = Signal();
    WaitForFenceValue(fenceValueForSignal);
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue::GetD3D12CommandQueue() const
{
    return m_pCommandQueue;
}

Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator()
{
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
    HRESULT res = m_pDevice->CreateCommandAllocator(m_commandListType, IID_PPV_ARGS(&commandAllocator));
    ThrowIfFailed(res);

    return commandAllocator;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CommandQueue::CreateCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator)
{
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList;
    HRESULT res = m_pDevice->CreateCommandList(0, m_commandListType, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    ThrowIfFailed(res);

    return commandList;
}
