/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

struct ID3D12Device2;

namespace Rendering
{
	class DeviceImpl;

	class Device
	{
	public:
		Device();
		~Device();

		void Init();
		void Shutdown();

		//this function should be removed once dx12 is nicely hidden inside Rendering.
		ID3D12Device2* GetDx12Device();

	private:
		DeviceImpl* m_pImpl;
	};
}
