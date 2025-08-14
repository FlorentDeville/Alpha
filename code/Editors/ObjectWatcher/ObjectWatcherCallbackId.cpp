/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/ObjectWatcher/ObjectWatcherCallbackId.h"

namespace Editors
{
	ObjectWatcherCallbackId::ObjectWatcherCallbackId()
		: m_pObj(nullptr)
		, m_cid()
	{ }

	ObjectWatcherCallbackId::ObjectWatcherCallbackId(void* pObj, Core::CallbackId cid)
		: m_pObj(pObj)
		, m_cid(cid)
	{ }

	ObjectWatcherCallbackId::~ObjectWatcherCallbackId()
	{ }

	bool ObjectWatcherCallbackId::IsValid() const
	{
		return m_cid.IsValid() && m_pObj; 
	}
}
