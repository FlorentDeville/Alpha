/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#define NOMINMAX

#include "CommandQueue.h"

#include "Helper.h"

#include <assert.h>
#include <chrono>

CommandQueue::CommandQueue(ID3D12Device2* pDevice, D3D12_COMMAND_LIST_TYPE type)
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

    while(!m_commandListQueue.empty())
    {
        ID3D12GraphicsCommandList2* pList = m_commandListQueue.front();
        pList->Release();
        m_commandListQueue.pop();
    }

    while (!m_commandAllocatorQueue.empty())
    {
        CommandAllocatorEntry& entry = m_commandAllocatorQueue.front();
        entry.pCommandAllocator->Release();
        m_commandAllocatorQueue.pop();
    }

    m_pCommandQueue->Release();
    m_pFence->Release();
}

ID3D12GraphicsCommandList2* CommandQueue::GetCommandList()
{
    ID3D12CommandAllocator* pCommandAllocator;
    ID3D12GraphicsCommandList2* pCommandList;

    if (!m_commandAllocatorQueue.empty() && IsFenceComplete(m_commandAllocatorQueue.front().fenceValue))
    {
        pCommandAllocator = m_commandAllocatorQueue.front().pCommandAllocator;
        m_commandAllocatorQueue.pop();

        ThrowIfFailed(pCommandAllocator->Reset());
    }
    else
    {
        pCommandAllocator = CreateCommandAllocator();
    }

    if (!m_commandListQueue.empty())
    {
        pCommandList = m_commandListQueue.front();
        m_commandListQueue.pop();

        ThrowIfFailed(pCommandList->Reset(pCommandAllocator, nullptr));
    }
    else
    {
        pCommandList = CreateCommandList(pCommandAllocator);
    }

    // Associate the command allocator with the command list so that it can be
    // retrieved when the command list is executed.
    ThrowIfFailed(pCommandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), pCommandAllocator));

    return pCommandList;
}

uint64_t CommandQueue::ExecuteCommandList(ID3D12GraphicsCommandList2* pCommandList)
{
    pCommandList->Close();

    ID3D12CommandAllocator* pCommandAllocator = nullptr;
    UINT dataSize = sizeof(pCommandAllocator);
    ThrowIfFailed(pCommandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &pCommandAllocator));

    ID3D12CommandList* const ppCommandLists[] = {
        pCommandList
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
    ThrowIfFailed(m_pCommandQueue->Signal(m_pFence, fenceValueForSignal));

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

ID3D12CommandQueue* CommandQueue::GetD3D12CommandQueue() const
{
    return m_pCommandQueue;
}

ID3D12CommandAllocator* CommandQueue::CreateCommandAllocator()
{
    ID3D12CommandAllocator* pCommandAllocator;
    HRESULT res = m_pDevice->CreateCommandAllocator(m_commandListType, IID_PPV_ARGS(&pCommandAllocator));
    ThrowIfFailed(res);

    return pCommandAllocator;
}

ID3D12GraphicsCommandList2* CommandQueue::CreateCommandList(ID3D12CommandAllocator* pAllocator)
{
    ID3D12GraphicsCommandList2* pCommandList;
    HRESULT res = m_pDevice->CreateCommandList(0, m_commandListType, pAllocator, nullptr, IID_PPV_ARGS(&pCommandList));
    ThrowIfFailed(res);

    return pCommandList;
}
