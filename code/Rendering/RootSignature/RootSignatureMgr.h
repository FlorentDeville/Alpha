/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <string>
#include <vector>

#include "Core/Singleton.h"

namespace Rendering
{
	struct RootSignatureId;
	class RootSignature;

	enum class EngineRootSigs
	{
		SHADOWMAP_SPOTLIGHT,
		SHADOWMAP_DIRLIGHT,
		OBJECTID,
		COPY_RENDER_TARGET,

		COUNT
	};

	class RootSignatureMgr : public Core::Singleton<RootSignatureMgr>
	{
	public:
		RootSignatureMgr();
		~RootSignatureMgr();

		void Init(const std::string&);
		void Shutdown();

		RootSignature* CreateRootSignature(const std::string& path, RootSignatureId& id);

		RootSignature* GetRootSignature(RootSignatureId id) const;
		RootSignature* GetRootSignature(EngineRootSigs id) const;

	private:
		std::vector<RootSignature*> m_rootSignatures;
	};
}
