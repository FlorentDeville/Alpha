/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Rendering
{
	class BufferHandle;

	namespace Internal
	{
		class Dx12Buffer;

		namespace BufferManager
		{
			Dx12Buffer* CreateBuffer(BufferHandle& handle);
			void ReleaseBuffer(const BufferHandle& handle);
			Dx12Buffer* GetBuffer(const BufferHandle& handle);
		};
	}
}
