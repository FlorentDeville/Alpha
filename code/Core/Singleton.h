/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Core
{
	template<class T> class Singleton
	{
	public:
		static T& Get()
		{
			return *s_singleton;
		}

		static T& InitSingleton()
		{
			s_singleton = new T();
			return *s_singleton;
		}

		static void ReleaseSingleton()
		{
			delete s_singleton;
			s_singleton = nullptr;
		}

		virtual void Init(){}
		virtual void Shutdown(){}
		
	protected:
		Singleton(){}
		virtual ~Singleton(){}

	private:
		static T* s_singleton;
	};

	template<class T> T* Singleton<T>::s_singleton = nullptr;
}