/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Resource/Resource.h"

Resource::Resource(const std::string& name)
	:m_name(name)
{}

Resource::~Resource()
{}

void Resource::Release()
{}
