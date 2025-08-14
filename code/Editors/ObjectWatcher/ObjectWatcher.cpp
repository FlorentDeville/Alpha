/********************************************************************/
/* � 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
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
		void* pFieldPtr = pField->GetDataPtr(pObj);
		pField->GetType()->Copy(pValue, pFieldPtr);

		std::map<Systems::Object*, WatcherCallbackList>::const_iterator it = m_watchers.find(pObj);
		if (it == m_watchers.cend())
			return;

		const WatcherCallbackList& callbacks = it->second;
		callbacks(pObj, pField, OPERATION::SET_FIELD, 0);
	}

	void ObjectWatcher::SetArrayFieldValue(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index, const void* pValue)
	{
		assert(pField->GetType()->IsContainer());

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
		assert(pField->GetType()->IsContainer());

		Core::BaseArray* pArray = pField->GetDataPtr<Core::BaseArray>(pObj);
		
		uint32_t newIndex = pArray->GetSize();
		pArray->AddElement();

		if(pValue)
			Internal_SetArrayFieldValue(pArray, pField, newIndex, pValue);

		const WatcherCallbackList* callbacks = FindWatcherCallback(pObj);
		if (!callbacks)
			return;

		(*callbacks)(pObj, pField, OPERATION::ADD_ELEMENT, pArray->GetSize() -1);
	}

	void ObjectWatcher::RemoveArrayElement(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
	{
		const Systems::TypeDescriptor* pArrayType = pField->GetType();

		assert(pArrayType->IsContainer());

		Core::BaseArray* pArray = pField->GetDataPtr<Core::BaseArray>(pObj);

		if (pArrayType->IsElementPointer())
		{
			void* pElement = pArray->GetElement(index); //pElement is actually a T** so I need to dereference it
			uint64_t* pElementPtr = reinterpret_cast<uint64_t*>(pElement);
			pElementPtr = reinterpret_cast<uint64_t*>(*pElementPtr);
			pArrayType->GetElementType()->Destruct(pElementPtr);
		}
		
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

	void ObjectWatcher::SendFieldModifiedEvent(Systems::Object* pObj, const Systems::FieldDescriptor* pField, OPERATION op, uint32_t index)
	{
		const WatcherCallbackList* pCallbacks = FindWatcherCallback(pObj);
		if (!pCallbacks)
			return;

		(*pCallbacks)(pObj, pField, op, index);
	}

	ObjectWatcherCallbackId ObjectWatcher::AddWatcher(const Systems::Object* pObj, const WatcherCallback& callback)
	{
		//ugly const cast here but I can't think of another solution
		Systems::Object* nonConstObj = const_cast<Systems::Object*>(pObj);

		Core::CallbackId id = m_watchers[nonConstObj].Connect(callback);
		return ObjectWatcherCallbackId(nonConstObj, id);
	}

	ObjectWatcherCallbackId ObjectWatcher::AddWatcher(Systems::Object* pObj, const WatcherCallback& callback)
	{
		Core::CallbackId id = m_watchers[pObj].Connect(callback);
		return ObjectWatcherCallbackId(pObj, id);
	}

	void ObjectWatcher::RemoveWatcher(ObjectWatcherCallbackId callbackId)
	{
		std::map<Systems::Object*, WatcherCallbackList>::iterator it = m_watchers.find(reinterpret_cast<Systems::Object*>(callbackId.m_pObj));
		if (it == m_watchers.end())
			return;

		WatcherCallbackList& callbacks = it->second;
		callbacks.Disconnect(callbackId.m_cid);
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

		const Systems::TypeDescriptor* pArrayType = pField->GetType();
		if (pArrayType->IsElementPointer())
		{
			//if it's an array of pointer, just copy the pointer
			uint64_t* pDst = static_cast<uint64_t*>(pArrayElement);
			uint64_t src = reinterpret_cast<uint64_t>(pValue);
			*pDst = src;
		}
		else
		{
			//if it's an array of element, copy the element
			pField->GetType()->GetElementType()->Copy(pValue, pArrayElement);
		}
	}
}
