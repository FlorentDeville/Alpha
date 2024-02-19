/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Systems
{
	class ComponentPool
	{
	public:
		static const int MAX_ENTITIES = 4096; //random number

		ComponentPool();
		~ComponentPool();
		
		void Init(size_t elementSize);

		void* Get(size_t index);

	private:
		size_t m_elementSize;
		char* m_pData;
	};
}
