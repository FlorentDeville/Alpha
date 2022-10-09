/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>
#include <vector>

#include "Core/Singleton.h"

struct RootSignatureId;
class RootSignature;

namespace Rendering
{
	class RootSignatureMgr : public Core::Singleton<RootSignatureMgr>
	{
	public:
		RootSignatureMgr();
		~RootSignatureMgr();

		RootSignatureId CreateRootSignature(const std::string& path);

		RootSignature* GetRootSignature(RootSignatureId id) const;

	private:
		std::vector<RootSignature*> m_rootSignatures;
	};
}
