/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/Internal/Buffer/BufferManager.h"

#include "Core/Collections/Array.h"

#include "Rendering/BufferHandle.h"
#include "Rendering/Internal/Buffer/Dx12Buffer.h"

namespace Rendering
{
	namespace Internal
	{
		namespace BufferManager
		{
			static Core::Array<Rendering::Internal::Dx12Buffer> g_buffers;

			static uint32_t FindFreeBuffer()
			{
				for (uint32_t ii = 0; ii < g_buffers.GetSize(); ++ii)
				{
					if (!g_buffers[ii].m_isUsed)
						return ii;
				}

				uint32_t newSize = g_buffers.GetSize() + (g_buffers.GetSize() / 2) + 1;
				uint32_t index = g_buffers.GetSize();
				g_buffers.Resize(newSize);
				return index;
			}

			Dx12Buffer* CreateBuffer(BufferHandle& handle)
			{
				uint32_t index = FindFreeBuffer();

				Dx12Buffer* pBuffer = &g_buffers[index];
				++pBuffer->m_generation;
				pBuffer->m_isUsed = true;

				handle.m_index = index;
				handle.m_generation = pBuffer->m_generation;

				return pBuffer;
			}

			void ReleaseBuffer(const BufferHandle& handle)
			{
				if (!g_buffers.IsValidIndex(handle.m_index))
					return;

				Dx12Buffer* pBuffer = &g_buffers[handle.m_index];
				if (!pBuffer->m_isUsed || pBuffer->m_generation != handle.m_generation)
					return;

				if (pBuffer->m_pResource)
				{
					pBuffer->m_pResource->Release();
					pBuffer->m_pResource = nullptr;
				}
				pBuffer->m_isUsed = false;
			}

			Dx12Buffer* GetBuffer(const BufferHandle& handle)
			{
				if (!g_buffers.IsValidIndex(handle.m_index))
					return nullptr;

				Dx12Buffer* pBuffer = &g_buffers[handle.m_index];
				if (pBuffer->m_generation != handle.m_generation)
					return nullptr;

				return pBuffer;
			}
		};
	}
}
