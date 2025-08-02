/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/Reflection/FieldDescriptor.h"

namespace Editors
{
	ObjectWatcher::ObjectWatcher()
		: Singleton<ObjectWatcher>()
		, m_watchers()
	{ }

	ObjectWatcher::~ObjectWatcher()
	{ }

	void ObjectWatcher::Init()
	{ }

	void ObjectWatcher::Shutdown()
	{
		m_watchers.clear();
	}

	void ObjectWatcher::SetFieldValue(Systems::Object* pObj, const Systems::FieldDescriptor* pField, const void* pValue)
	{
		if (pField->GetType()->GetSid() == CONSTSID("std::string"))
		{
			//special case for std::string because it allocates memory, I can't just do a memcpy
			std::string* pDst = pField->GetDataPtr<std::string>(pObj);
			*pDst = *static_cast<const std::string*>(pValue);
		}
		else
		{
			void* pFieldPtr = pField->GetDataPtr(pObj);
			memcpy(pFieldPtr, pValue, pField->GetType()->GetSize());
		}

		std::map<Systems::Object*, WatcherCallbackList>::const_iterator it = m_watchers.find(pObj);
		if (it == m_watchers.cend())
			return;

		const WatcherCallbackList& callbacks = it->second;
		callbacks(pObj, pField, OPERATION::SET_FIELD, 0);
	}

	void ObjectWatcher::SetArrayFieldValue(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index, const void* pValue)
	{
		assert(pField->IsContainer());

		Core::BaseArray* pArray = pField->GetDataPtr<Core::BaseArray>(pObj);
		Internal_SetArrayFieldValue(pArray, pField, index, pValue);

		std::map<Systems::Object*, WatcherCallbackList>::const_iterator it = m_watchers.find(pObj);
		if (it == m_watchers.cend())
			return;

		const WatcherCallbackList* callbacks = FindWatcherCallback(pObj);
		if (!callbacks)
			return;

		(*callbacks)(pObj, pField, OPERATION::SET_ELEMENT, index);
	}

	void ObjectWatcher::AddArrayElement(Systems::Object* pObj, const Systems::FieldDescriptor* pField, const void* pValue)
	{
		assert(pField->IsContainer());

		Core::BaseArray* pArray = pField->GetDataPtr<Core::BaseArray>(pObj);
		
		uint32_t newIndex = pArray->GetSize();
		pArray->AddElement();

		Internal_SetArrayFieldValue(pArray, pField, newIndex, pValue);

		const WatcherCallbackList* callbacks = FindWatcherCallback(pObj);
		if (!callbacks)
			return;

		(*callbacks)(pObj, pField, OPERATION::ADD_ELEMENT, pArray->GetSize() -1);
	}

	void ObjectWatcher::RemoveArrayElement(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
	{
		assert(pField->IsContainer());

		Core::BaseArray* pArray = pField->GetDataPtr<Core::BaseArray>(pObj);

		pArray->RemoveElement(index);

		const WatcherCallbackList* callbacks = FindWatcherCallback(pObj);
		if (!callbacks)
			return;

		(*callbacks)(pObj, pField, OPERATION::REMOVE_ELEMENT, index);
	}

	void ObjectWatcher::ModifyField(Systems::Object* pObj, const Systems::FieldDescriptor* pField, OPERATION op, uint32_t index, const void* pValue)
	{
		switch (op)
		{
		case OPERATION::SET_FIELD:
			SetFieldValue(pObj, pField, pValue);
			break;

		case OPERATION::SET_ELEMENT:
			SetArrayFieldValue(pObj, pField, index, pValue);
			break;

		case OPERATION::ADD_ELEMENT:
			AddArrayElement(pObj, pField, pValue);
			break;

		case OPERATION::REMOVE_ELEMENT:
			RemoveArrayElement(pObj, pField, index);
			break;

		default:
			assert(false); // unsupported operation
			break;
		}
	}

	Core::CallbackId ObjectWatcher::AddWatcher(Systems::Object* pObj, const WatcherCallback& callback)
	{
		Core::CallbackId id = m_watchers[pObj].Connect(callback);
		return id;
	}

	void ObjectWatcher::RemoveWatcher(Systems::Object* pObj, Core::CallbackId callbackId)
	{
		std::map<Systems::Object*, WatcherCallbackList>::iterator it = m_watchers.find(pObj);
		if (it == m_watchers.end())
			return;

		WatcherCallbackList& callbacks = it->second;
		callbacks.Disconnect(callbackId);
	}

	const ObjectWatcher::WatcherCallbackList* ObjectWatcher::FindWatcherCallback(Systems::Object* pObj) const
	{
		std::map<Systems::Object*, WatcherCallbackList>::const_iterator it = m_watchers.find(pObj);
		if (it == m_watchers.cend())
			return nullptr;

		return &(it->second);
	}

	void ObjectWatcher::Internal_SetArrayFieldValue(Core::BaseArray* pArray, const Systems::FieldDescriptor* pField, uint32_t index, const void* pValue)
	{
		void* pArrayElement = pArray->GetElement(index);

		if (pField->GetType()->GetSid() == CONSTSID("std::string"))
		{
			//special case for std::string because it allocates memory, I can't just do a memcpy
			std::string* pDst = static_cast<std::string*>(pArrayElement);
			*pDst = *static_cast<const std::string*>(pValue);
		}
		else
		{
			memcpy(pArrayElement, pValue, pField->GetElementType()->GetSize());
		}
	}
}
