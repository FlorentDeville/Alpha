/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

class Resource
{
public:
	Resource(const std::string& name);
	virtual ~Resource();

	virtual void Release();

protected:
	std::string m_name;
};