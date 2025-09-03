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
		NewAssetType(const std::string& name, Core::Sid sid, Core::Sid classNameSid);

		~NewAssetType();

		const std::string& GetName() const;
		Core::Sid GetSid() const;
		Core::Sid GetClassName() const;

	private:
		std::string m_name;			// Name of the asset type as shown in the metadata
		Core::Sid m_sid;			// Sid of m_name

		Core::Sid m_classNameSid;	// Full name of the c++ class (with namespace)
	};
}
