/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackList.h"
#include "Core/Singleton.h"

#include <map>
#include <string>

namespace Core
{
	class BaseArray;
}

namespace Systems
{
	class FieldDescriptor;
	class Object;
}

namespace Editors
{
	class ObjectWatcher : public Core::Singleton<ObjectWatcher>
	{
	public:

		enum OPERATION
		{
			SET_FIELD,			// set the field to a new value
			SET_ELEMENT,		// the field is an array, set an element of the array to a new value
			ADD_ELEMENT,		// the field is an array, push back a new element to the array
			REMOVE_ELEMENT,		// the field is an array, remove an element from the array

			COUNT
		};

		// Event triggered when a field is modified.
		// Systems::Object* : the object that was modified
		// const Systems::FieldDescriptor* : the modified field
		// OPERATION : how the field was modified
		// uint32_t : the index of the modified variable in the field. Only usefull for arrays.
		using WatcherCallbackList = Core::CallbackList<void(Systems::Object*, const Systems::FieldDescriptor*, OPERATION, uint32_t)>;
		using WatcherCallback = WatcherCallbackList::Callback;

		ObjectWatcher();
		~ObjectWatcher();

		//Singleton interface
		void Init() override;
		void Shutdown() override;

		//Operations
		void SetFieldValue(Systems::Object* pObj, const Systems::FieldDescriptor* pField, const void* pValue);
		void SetArrayFieldValue(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index, const void* pValue);
		void AddArrayElement(Systems::Object* pObj, const Systems::FieldDescriptor* pField, const void* pValue);
		void RemoveArrayElement(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index);

		// Generic function, can do any operation
		void ModifyField(Systems::Object* pObj, const Systems::FieldDescriptor* pField, OPERATION op, uint32_t index, const void* pValue);

		Core::CallbackId AddWatcher(Systems::Object* pObj, const WatcherCallback& callback);
		Core::CallbackId AddWatcher(const Systems::Object* pObj, const WatcherCallback& callback);
		void RemoveWatcher(Systems::Object* pObj, Core::CallbackId callbackId);

	private:
		std::map<Systems::Object*, WatcherCallbackList> m_watchers;

		const WatcherCallbackList* FindWatcherCallback(Systems::Object* pObj) const;

		void Internal_SetArrayFieldValue(Core::BaseArray* pArray, const Systems::FieldDescriptor* pField, uint32_t index, const void* pValue);
	};
}
