#pragma once
#include "memory_manager.h"

namespace libertyFourXYZ {

	template<typename _TypeKey, typename _TypeEntry, size_t _Alignment,typename _TypeNum = size_t> class CDictionary {
	public:
		/*template<typename _TypeKey, typename _TypeEntry> */class Entry {
		public:
			_TypeKey m_key;
			_TypeEntry m_data;
		};

		Entry *m_entries;
		_TypeNum m_size;
		_TypeNum m_capacity;
		
	};

}