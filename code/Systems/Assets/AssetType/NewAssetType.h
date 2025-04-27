/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Sid/Sid.h"

#include <string>

namespace Systems
{
	class NewAssetType
	{
	public:
		NewAssetType();
		NewAssetType(const std::string& name);
		NewAssetType(const std::string& name, Core::Sid sid);

		~NewAssetType();

		const std::string& GetName() const;
		Core::Sid GetSid() const;

	private:
		std::string m_name;
		Core::Sid m_sid;
	};
}
